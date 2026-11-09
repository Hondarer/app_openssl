#!/usr/bin/env python3
"""OpenSSL ビルドスクリプトの引数、ハッシュ、ツール判定を検証する。"""

import io
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "bin_internal"))
import build_openssl


class BuildOpensslTest(unittest.TestCase):
    def test_feature_args_keep_static_asm_and_protocols(self):
        self.assertEqual(
            [
                "no-shared",
                "no-pinshared",
                "no-autoload-config",
                "no-apps",
                "no-docs",
                "no-makedepend",
                "enable-tls1_3",
                "enable-dtls1_2",
            ],
            build_openssl.FEATURE_ARGS,
        )
        self.assertNotIn("no-asm", build_openssl.FEATURE_ARGS)
        self.assertNotIn("no-pic", build_openssl.FEATURE_ARGS)

    def test_configure_args_linux_release(self):
        args = build_openssl.configure_args(
            "linux",
            "x86_64",
            "RelWithDebInfo",
            Path("/opt/openssl/install"),
            Path("/opt/openssl/install/ssl"),
        )
        self.assertEqual("linux-x86_64", args[0])
        self.assertIn("no-shared", args)
        self.assertIn("no-pinshared", args)
        self.assertIn("enable-tls1_3", args)
        self.assertIn("enable-dtls1_2", args)
        self.assertIn("--prefix=/opt/openssl/install", args)
        self.assertIn("--libdir=lib", args)
        self.assertIn("--openssldir=/opt/openssl/install/ssl", args)
        self.assertIn("no-autoload-config", args)
        # PIC は OpenSSL の既定に任せ、assert_pic で生成結果を確認する。
        self.assertNotIn("-fPIC", args)
        self.assertNotIn("--debug", args)
        self.assertNotIn("no-asm", args)

    def test_configure_args_windows_debug_and_rejected_arch(self):
        args = build_openssl.configure_args(
            "windows",
            "AMD64",
            "Debug",
            Path("C:/src/openssl/install"),
            Path("C:/src/openssl/install/ssl"),
        )
        self.assertEqual("VC-WIN64A", args[0])
        self.assertIn("--debug", args)
        self.assertNotIn("-fPIC", args)
        with self.assertRaises(build_openssl.BuildError):
            build_openssl.configure_args(
                "windows",
                "x86",
                "Release",
                Path("C:/src/openssl/install"),
                Path("C:/src/openssl/install/ssl"),
            )

    def test_configure_args_reject_space_and_unknown_config(self):
        with self.assertRaises(build_openssl.BuildError):
            build_openssl.configure_args(
                "linux",
                "x86_64",
                "RelWithDebInfo",
                Path("/opt/my openssl/install"),
                Path("/opt/my openssl/install/ssl"),
            )
        with self.assertRaises(build_openssl.BuildError):
            build_openssl.configure_args(
                "linux",
                "aarch64",
                "DebugInfo",
                Path("/opt/openssl/install"),
                Path("/opt/openssl/install/ssl"),
            )

    def test_select_package_name(self):
        known = {
            "openssl-3.5.8.tar.gz": "aa",
            "openssl-3.5.9.tar.gz": "bb",
        }
        selected, rejected = build_openssl.select_package_name(
            ["openssl-3.5.8.tar.gz", "openssl-3.5.9.tar.gz"],
            known,
        )
        self.assertEqual("openssl-3.5.9.tar.gz", selected)
        self.assertEqual(["openssl-3.5.8.tar.gz"], rejected)
        with self.assertRaises(build_openssl.BuildError) as missing:
            build_openssl.select_package_name([], known)
        self.assertIn(build_openssl.UPSTREAM_URL, str(missing.exception))
        with self.assertRaises(build_openssl.BuildError) as unknown:
            build_openssl.select_package_name(["openssl-3.5.10.tar.gz"], known)
        self.assertIn("openssl-3.5.10.tar.gz", str(unknown.exception))

    def test_hash_mismatch_stops_before_extract(self):
        with tempfile.TemporaryDirectory(prefix="openssl-hash-") as temporary:
            app = Path(temporary)
            packages = app / "packages"
            packages.mkdir()
            (packages / "openssl-3.5.9.tar.gz").write_bytes(b"not-an-archive")
            stderr = io.StringIO()
            with mock.patch("sys.stderr", stderr):
                code = build_openssl.main(["--app-dir", str(app), "--config", "RelWithDebInfo"])
            self.assertEqual(1, code)
            self.assertIn("SHA-256", stderr.getvalue())
            self.assertFalse((app / "gen" / "src").exists())

    def test_missing_package_message(self):
        with tempfile.TemporaryDirectory(prefix="openssl-missing-") as temporary:
            stderr = io.StringIO()
            with mock.patch("sys.stderr", stderr):
                code = build_openssl.main(["--app-dir", temporary])
            self.assertEqual(1, code)
            self.assertIn("curl", stderr.getvalue())

    def test_filter_makeflags(self):
        self.assertEqual("", build_openssl.filter_makeflags(""))
        self.assertEqual(
            "-j --jobserver-auth=3,4",
            build_openssl.filter_makeflags("-j --jobserver-auth=3,4 --no-print-directory"),
        )
        self.assertEqual("--jobs=8", build_openssl.filter_makeflags("--jobs=8 -k"))
        self.assertEqual("-j72", build_openssl.filter_makeflags(" -j72 "))
        self.assertEqual(
            "--jobserver-fds=4,5",
            build_openssl.filter_makeflags("--jobserver-fds=4,5"),
        )

    def test_child_env_strips_makefw_flags(self):
        base = {
            "CFLAGS": "-fvisibility=hidden",
            "MAKEFLAGS": "-j --jobserver-auth=3,4 --no-print-directory",
            "MFLAGS": "-j",
            "MAKELEVEL": "2",
            "PATH": "/usr/bin",
            "PERL": "/usr/bin/perl",
        }
        make_env = build_openssl.child_env("make", base)
        self.assertNotIn("CFLAGS", make_env)
        self.assertNotIn("PERL", make_env)
        self.assertNotIn("MFLAGS", make_env)
        self.assertEqual("2", make_env["MAKELEVEL"])
        nmake_env = build_openssl.child_env("nmake", base)
        self.assertNotIn("MAKEFLAGS", nmake_env)
        self.assertNotIn("MAKELEVEL", nmake_env)
        configure_env = build_openssl.child_env("configure", base)
        self.assertNotIn("MAKEFLAGS", configure_env)
        self.assertNotIn("CFLAGS", configure_env)
        # Configure は環境変数 PERL を $^X より優先して Makefile に記録する。
        self.assertNotIn("PERL", configure_env)

    def test_parallel_make_flags_share_parent_jobserver(self):
        # ジョブ サーバーがあれば、親と同じジョブ枠を共有する。
        self.assertEqual(
            "-j8 --jobserver-auth=3,4",
            build_openssl.parallel_make_flags("-j8 --jobserver-auth=3,4 --no-print-directory"),
        )
        self.assertEqual(
            "--jobserver-auth=fifo:/tmp/GMfifo1",
            build_openssl.parallel_make_flags("--jobserver-auth=fifo:/tmp/GMfifo1"),
        )
        # ジョブ サーバーが無ければ、-jN の数だけを使う。
        self.assertEqual("-j4", build_openssl.parallel_make_flags("-j4 -k"))
        # 数値の無い -j は無制限になるため、CPU 数に置き換える。
        cpu = f"-j{build_openssl.os.cpu_count() or 1}"
        self.assertEqual(cpu, build_openssl.parallel_make_flags("-j"))
        self.assertEqual(cpu, build_openssl.parallel_make_flags(""))

    def test_find_perl_uses_platform_command(self):
        seen = []

        def which(name):
            seen.append(name)
            return f"C:/devbin/{name}.exe"

        self.assertEqual(
            "C:/devbin/strawberry-perl.exe",
            build_openssl.find_perl("windows", which),
        )
        self.assertEqual("/usr/bin/perl", build_openssl.find_perl("linux", lambda name: "/usr/bin/" + name))
        self.assertEqual(["strawberry-perl"], seen)

    def test_find_perl_missing_on_windows_mentions_devbin(self):
        with self.assertRaises(build_openssl.BuildError) as caught:
            build_openssl.find_perl("windows", lambda name: None)
        message = str(caught.exception)
        self.assertIn("strawberry-perl", message)
        self.assertIn(build_openssl.DEVBIN_WIN_URL, message)
        with self.assertRaises(build_openssl.BuildError) as linux:
            build_openssl.find_perl("linux", lambda name: None)
        self.assertIn("NOTES-PERL.md", str(linux.exception))

    def test_assert_pic_reads_library_flags(self):
        with tempfile.TemporaryDirectory(prefix="openssl-pic-") as temporary:
            source = Path(temporary)
            makefile = source / "Makefile"
            makefile.write_bytes(
                b"CFLAGS=-Wall -O3\n"
                b"LIB_CPPFLAGS=-DL_ENDIAN -DOPENSSL_PIC $(CPPFLAGS)\n"
                b"LIB_CFLAGS=-fPIC $(CNF_CFLAGS) $(CFLAGS)\n"
            )
            build_openssl.assert_pic(source)
            # LIB_CFLAGS 以外にだけ -fPIC があっても、ライブラリは PIC にならない。
            makefile.write_bytes(
                b"CFLAGS=-Wall -O3 -fPIC\n"
                b"LIB_CPPFLAGS=-DL_ENDIAN $(CPPFLAGS)\n"
                b"LIB_CFLAGS=$(CNF_CFLAGS) $(CFLAGS)\n"
            )
            with self.assertRaises(build_openssl.BuildError):
                build_openssl.assert_pic(source)

    def test_config_header_requires_autoload_config_disabled(self):
        with tempfile.TemporaryDirectory(prefix="openssl-config-") as temporary:
            header = Path(temporary) / "configuration.h"
            header.write_bytes(b"# define OPENSSL_NO_AUTOLOAD_CONFIG\n")
            build_openssl.assert_config_header(header)
            header.write_bytes(b"# define OPENSSL_NO_EGD\n")
            with self.assertRaises(build_openssl.BuildError) as caught:
                build_openssl.assert_config_header(header)
            self.assertIn("OPENSSL_NO_AUTOLOAD_CONFIG", str(caught.exception))

    def test_nasm_version_parser(self):
        self.assertEqual((2, 15, 5), build_openssl.parse_nasm_version(
            "NASM version 2.15.05 compiled on Aug 28 2020"
        ))
        self.assertEqual((2, 13, 3), build_openssl.parse_nasm_version("NASM version 2.13.3"))
        self.assertLess(
            build_openssl.parse_nasm_version("NASM version 2.13.02"),
            build_openssl.MIN_NASM,
        )
        with self.assertRaises(build_openssl.BuildError):
            build_openssl.require_nasm((2, 13, 2))
        build_openssl.require_nasm((2, 13, 3))

    def test_missing_perl_module_mentions_perl_core(self):
        def fail_run(cmd, **kwargs):
            self.assertEqual("utf-8", kwargs.get("encoding"))
            self.assertEqual("/opt/perl/bin/perl", cmd[0])
            name = cmd[1][2:]
            code = 2 if name == "Time::Piece" else 0
            return subprocess.CompletedProcess(cmd, code, "", "")

        with self.assertRaises(build_openssl.BuildError) as caught:
            build_openssl.require_perl_modules("/opt/perl/bin/perl", fail_run)
        self.assertIn("Time::Piece", str(caught.exception))
        self.assertIn("perl-core", str(caught.exception))

        def ok_run(cmd, **kwargs):
            return subprocess.CompletedProcess(cmd, 0, "", "")

        build_openssl.require_perl_modules("/opt/perl/bin/perl", ok_run)

    def test_windows_perl_rejects_msys(self):
        build_openssl.require_perl((5, 26, 3), "linux", "linux")
        build_openssl.require_perl((5, 32, 1), "MSWin32", "windows")
        with self.assertRaises(build_openssl.BuildError) as caught:
            build_openssl.require_perl((5, 32, 1), "msys", "windows")
        self.assertIn("MSWin32", str(caught.exception))
        self.assertIn("strawberry-perl", str(caught.exception))
        with self.assertRaises(build_openssl.BuildError):
            build_openssl.require_perl((5, 8, 9), "linux", "linux")

    def test_drop_unsupported_enables_keeps_required_options(self):
        args = list(build_openssl.FEATURE_ARGS)
        reduced = build_openssl.drop_unsupported_enables(
            args,
            b"***** Unsupported options: enable-tls1_3, enable-dtls1_2\n",
        )
        self.assertNotIn("enable-tls1_3", reduced)
        self.assertNotIn("enable-dtls1_2", reduced)
        self.assertIn("no-shared", reduced)
        self.assertIn("no-pinshared", reduced)
        self.assertIsNone(build_openssl.drop_unsupported_enables(args, b"some other error"))

    def test_clean_removes_products_and_keeps_include_gitignore(self):
        with tempfile.TemporaryDirectory(prefix="openssl-clean-") as temporary:
            app = Path(temporary)
            include = app / "prod" / "include"
            include.mkdir(parents=True)
            (include / ".gitignore").write_text(
                build_openssl.INCLUDE_GITIGNORE,
                encoding="utf-8",
                newline="\n",
            )
            headers = include / "openssl"
            headers.mkdir()
            (headers / "ssl.h").write_text("header\n", encoding="utf-8", newline="\n")
            lib = app / "prod" / "lib"
            lib.mkdir(parents=True)
            (lib / "libcrypto.a").write_bytes(b"crypto")
            (lib / "libssl.a").write_bytes(b"ssl")
            gen = app / "gen" / "src"
            gen.mkdir(parents=True)
            (gen / "Configure").write_text("configure\n", encoding="utf-8", newline="\n")
            code = build_openssl.main(["--app-dir", str(app), "--clean"])
            self.assertEqual(0, code)
            self.assertFalse((include / "openssl").exists())
            self.assertFalse((lib / "libcrypto.a").exists())
            self.assertFalse((app / "gen").exists())
            self.assertEqual(
                build_openssl.INCLUDE_GITIGNORE,
                (include / ".gitignore").read_text(encoding="utf-8"),
            )

    def test_defined_macros_ignore_comments_and_undef(self):
        data = b"\n".join([
            b"# define OPENSSL_NO_ASM",
            b"#  undef OPENSSL_NO_TLS1_3",
            b"/* # define OPENSSL_NO_DTLS */",
        ])
        defined = build_openssl.defined_macros(data)
        self.assertIn(b"OPENSSL_NO_ASM", defined)
        self.assertNotIn(b"OPENSSL_NO_TLS1_3", defined)


if __name__ == "__main__":
    unittest.main()
