#!/usr/bin/env python3
"""OpenSSL の公式アーカイブを Configure で共有ライブラリとしてビルドし、prod へ配置する。

makefw は OpenSSL のソースをコンパイルしない。アセンブルは OpenSSL 自身の
Makefile が行う。静的ライブラリは配置しない。
共有ライブラリの名前とシンボル バージョンには、shlib_variant で識別子 _app を付ける。

see: https://github.com/openssl/openssl/blob/openssl-3.5.9/INSTALL.md
see: https://github.com/openssl/openssl/blob/openssl-3.5.9/NOTES-WINDOWS.md
see: https://github.com/openssl/openssl/blob/openssl-3.5.9/NOTES-PERL.md
see: https://github.com/openssl/openssl/blob/openssl-3.5.9/Configurations/10-main.conf
see: https://github.com/openssl/openssl/blob/openssl-3.5.9/Configurations/README.md
see: https://github.com/openssl/openssl/blob/openssl-3.5.9/include/openssl/e_os2.h
"""

from contextlib import contextmanager
import argparse
import errno
import hashlib
import os
import platform as platform_mod
import re
import shutil
import subprocess
import sys
import tarfile
import time
from pathlib import Path


OPENSSL_VERSION = "3.5.9"
UPSTREAM_URL = (
    "https://github.com/openssl/openssl/releases/download/"
    "openssl-3.5.9/openssl-3.5.9.tar.gz"
)
# 取り込み時点の公式アーカイブ。ファイル名を増やすときは packages/README.md も更新する。
EXPECTED_SHA256 = {
    "openssl-3.5.9.tar.gz":
        "603f5602e2eef00d77fbd429d34dcd5822bb301757a1bc9cdb24c670f1eb859a",
}
PACKAGE_PATTERN = re.compile(r"^openssl-(\d+)\.(\d+)\.(\d+)\.tar\.gz$", re.IGNORECASE)
# shared は共有ライブラリを作る。
# no-autoload-config は、OPENSSLDIR の openssl.cnf を初期化時に自動で読まない。
# OPENSSLDIR はビルドした場所の絶対パスになるため、配布先でそのパスを作れる利用者が
# 設定を差し込めないようにする。
# no-padlockeng は、VIA の CPU 専用の PadLock エンジン (engines/e_padlock.c) を作らない。
# ENGINE API は OpenSSL 3.0 で非推奨であり、利用側は使わない。GCC 14 の -O3 では、
# padlock_cfb_cipher の 16 バイト未満の端数の処理に -Wstringop-overflow の誤検出が出る。
# enable-tls1_3 と enable-dtls1_2 は既定で有効な機能を明示する。
# see: https://github.com/openssl/openssl/blob/openssl-3.5.9/INSTALL.md
FEATURE_ARGS = [
    "shared",
    "no-autoload-config",
    "no-apps",
    "no-docs",
    "no-makedepend",
    "no-padlockeng",
    "enable-tls1_3",
    "enable-dtls1_2",
]
# Windows では Configure に osname が MSWin32 の Perl が必要になる。
# devbin-win は Strawberry Perl の perl.exe を strawberry-perl.exe に改名して PATH に置く。
# Git 付属の Perl (msys) を前提とするシェル スクリプトが perl で起動するものを変えないため。
# Configure は起動した Perl の絶対パス ($^X) を Makefile に記録するため、
# 以降の make や nmake も同じ Perl を使う。
# see: https://github.com/openssl/openssl/blob/openssl-3.5.9/Configure
PERL_COMMANDS = {
    "linux": "perl",
    "windows": "strawberry-perl",
}
DEVBIN_WIN_URL = "https://github.com/Hondarer/devbin-win"
# Windows (MSVC) で OpenSSL 本体のソースが出す警告への対処。Configure は、ターゲットの後ろに
# 書いたコンパイラー オプションを OpenSSL の既定の CFLAGS (/W3 /wd4090 ...) の後ろに追加する。
# 抑制する警告は、OpenSSL の公式の設定 (/W3) でも出る本体由来のもので、内容を確認したうえで抑制する。
# see: https://github.com/openssl/openssl/blob/openssl-3.5.9/INSTALL.md
WINDOWS_COMPILER_ARGS = [
    # C4267: size_t から int などへの変換。本体全体で多数ある。
    "-wd4267",
    # C4244: 整数の縮小変換。
    "-wd4244",
    # C4133: tsan_assist.h の tsan_add などが sizeof で 64/32 ビットの Interlocked 関数を選ぶ。
    # 実行されない 64 ビット側でも型が検査され、int のときに警告となる。
    "-wd4133",
    # C4334: 32 ビットのシフトの 64 ビットへの変換。quic_vlint.h のシフトは最大 1U << 3。
    "-wd4334",
    # C4319: ~ の結果のゼロ拡張。bn_gcd.c などのビット マスクの演算。
    "-wd4319",
    # C4819 への対処。日本語環境 (コード ページ 932) でも、ソースを UTF-8 として読む。
    # 警告の抑制ではなく、読み方を正す。実行文字セットは変えない。
    # see: https://learn.microsoft.com/en-us/cpp/build/reference/source-charset-set-source-character-set
    "-source-charset:utf-8",
]
MIN_PERL = (5, 10, 0)
# INSTALL.md のアセンブラー表。VAES に必要な NASM の下限。
MIN_NASM = (2, 13, 3)
ASM_MARKERS = {
    "linux-x86_64": (b"aesni", b"x86_64cpuid"),
    "linux-aarch64": (b"aesv8", b"armv8"),
    "VC-WIN64A": (b"aesni", b"x86_64cpuid"),
}
DISABLED_MACROS = (
    b"OPENSSL_NO_ASM",
    b"OPENSSL_NO_TLS1_3",
    b"OPENSSL_NO_DTLS",
    b"OPENSSL_NO_DTLS1_2",
)
REQUIRED_MACROS = (
    b"OPENSSL_NO_AUTOLOAD_CONFIG",
)
# 第三者管理ライブラリは、OS やほかの製品の同名ライブラリと区別するため、名前に接尾辞 _app を付ける。
# OpenSSL では、Configure のターゲット定義の shlib_variant にこの識別子を指定する。
# shlib_variant は、共有ライブラリのファイル名、SONAME、シンボル バージョンに反映される。
# シンボル バージョンは OPENSSL_<版> から OPENSSL_APP_<版> に変わるため、同じプロセスに
# OS の OpenSSL が読み込まれても、利用側の呼び出しが相手のライブラリへ束縛されない。
# see: https://github.com/openssl/openssl/blob/openssl-3.5.9/Configurations/README.md
SHLIB_VARIANT = "_app"
# 共有ライブラリの版の番号。OpenSSL の VERSION.dat の SHLIB_VERSION に対応する。
# see: https://github.com/openssl/openssl/blob/openssl-3.5.9/VERSION.dat
SHLIB_VERSION = "3"
SHLIB_BASE_NAMES = ("crypto", "ssl")
# shlib_variant から決まるシンボル バージョンの接頭辞と、付いてはならない標準の接頭辞。
VARIANT_SYMBOL_VERSION = f"OPENSSL_APP_{SHLIB_VERSION}.0.0".encode("ascii")
DEFAULT_SYMBOL_VERSION = f"OPENSSL_{SHLIB_VERSION}.0.0".encode("ascii")
# 以前の版が配置した成果物の名前。ビルドと clean で削除する。
LEGACY_PRODUCT_LIB_NAMES = (
    "libcrypto.a",
    "libssl.a",
    "libcrypto.lib",
    "libssl.lib",
    "libcrypto.dll",
    "libssl.dll",
    "libcrypto.pdb",
    "libssl.pdb",
    "ossl_static.pdb",
    "dso.pdb",
)
# 共有ライブラリとともに配布するライセンス文。アーカイブの LICENSE.txt を
# zlib や cjson と同じくライブラリのディレクトリへ配置する。NOTICE ファイルは同梱されない。
LICENSE_NAME = "LICENSE.txt"
LICENSE_RELATIVE = ("prod", "libsrc", "openssl", LICENSE_NAME)
INCLUDE_GITIGNORE = (
    "# Configure の install_dev が配置する OpenSSL 公開ヘッダー。直接編集しない。\n"
    "/openssl/\n"
)
_DEFINE_RE = re.compile(br"^\s*#\s*define\s+([A-Z0-9_]+)\b", re.MULTILINE)
_MAKEFLAG_RE = re.compile(
    r"(?:^|\s)(-j(?:\d+)?|--jobs=\d+|--jobserver-auth=\S+|--jobserver-fds=\S+)"
)
# PERL は Configure が $^X より優先して Makefile に記録するため、外から引き継がない。
_SANITIZE_ENV = (
    "CC", "CXX", "CFLAGS", "CXXFLAGS", "CPPFLAGS", "LDFLAGS",
    "AR", "RANLIB", "AS", "ASFLAGS", "LIBS", "DEFINES", "LD",
    "CL", "_CL_", "LINK", "PERL",
)


class BuildError(Exception):
    """ビルドを止める、利用者向けの失敗。"""


def configure_stdio():
    for stream in (sys.stdout, sys.stderr):
        reconfigure = getattr(stream, "reconfigure", None)
        if reconfigure is None:
            continue
        reconfigure(encoding="utf-8")


def filter_makeflags(flags):
    """親 make の MAKEFLAGS から、ジョブ サーバーと並列数だけを残す。"""
    if not flags:
        return ""
    return " ".join(match.group(1) for match in _MAKEFLAG_RE.finditer(flags))


def parallel_make_flags(raw):
    """子 make に渡す並列指定。

    親 make にジョブ サーバーがあれば、その指定をそのまま渡して同じジョブ枠を共有する。
    makelocal.mk の pre-build は行頭の + でジョブ サーバーを子へ渡し、
    run_logged は make の子プロセスで fd を閉じない。
    ジョブ サーバーが無く -jN だけがあればその数を使い、どちらも無いときは CPU 数を使う。
    数値の無い -j は無制限になるため使わない。
    see: https://www.gnu.org/software/make/manual/html_node/Job-Slots.html
    """
    tokens = filter_makeflags(raw).split()
    if any(token.startswith("--jobserver-") for token in tokens):
        return " ".join(tokens)
    counted = [token for token in tokens if re.fullmatch(r"-j\d+|--jobs=\d+", token)]
    if counted:
        return " ".join(counted)
    return f"-j{os.cpu_count() or 1}"


def child_env(kind, base=None):
    """OpenSSL の Configure / make / nmake に、makefw のフラグを渡さない。

    GNU make の子プロセスでは MAKELEVEL を残す。
    nmake は GNU make の MAKEFLAGS を解釈しない。
    """
    env = dict(os.environ if base is None else base)
    for key in _SANITIZE_ENV:
        env.pop(key, None)
    if kind == "make":
        env["MAKEFLAGS"] = parallel_make_flags(env.get("MAKEFLAGS", ""))
        env.pop("MFLAGS", None)
    elif kind in ("configure", "nmake"):
        env.pop("MAKEFLAGS", None)
        env.pop("MFLAGS", None)
        env.pop("MAKELEVEL", None)
    else:
        raise BuildError(f"未知の子プロセス種別です: {kind}")
    return env


def host_platform():
    if sys.platform.startswith("linux"):
        return "linux"
    if sys.platform.startswith("win"):
        return "windows"
    raise BuildError(f"未対応の OS です: {sys.platform}")


def target_name(platform, machine):
    machine_key = machine.lower()
    if platform == "linux":
        if machine_key in ("x86_64", "amd64"):
            return "linux-x86_64"
        if machine_key in ("aarch64", "arm64"):
            return "linux-aarch64"
        raise BuildError(f"未対応の Linux アーキテクチャです: {machine}")
    if platform == "windows":
        if machine_key in ("x86_64", "amd64"):
            return "VC-WIN64A"
        raise BuildError(
            "Windows は x64 (VC-WIN64A) だけをビルドします。"
            f" 検出: {machine}"
        )
    raise BuildError(f"未対応の OS です: {platform}")


def find_msvc_linker(which=None):
    """cl.exe と同じディレクトリにある MSVC の link.exe の絶対パス。

    VC-WIN64A の既定の LD は link で、nmake は PATH から探す。PATH 上で Git 付属の
    GNU coreutils の link (usr/bin/link.exe) が MSVC より先にあると、legacy プロバイダーの
    DLL などのリンクが "link: extra operand" で失敗する。Configure に LD を絶対パスで渡し、
    PATH の順序に依存しないようにする。
    see: https://github.com/openssl/openssl/blob/openssl-3.5.9/Configurations/10-main.conf
    see: https://github.com/openssl/openssl/blob/openssl-3.5.9/INSTALL.md#ld
    """
    if which is None:
        which = shutil.which
    cl = which("cl")
    if cl is None:
        raise BuildError(
            "cl が PATH にありません。"
            " Visual Studio の開発者コマンドプロンプトで実行してください。"
            " see: https://github.com/openssl/openssl/blob/openssl-3.5.9/NOTES-WINDOWS.md"
        )
    linker = Path(cl).resolve().parent / "link.exe"
    if not linker.is_file():
        raise BuildError(f"cl.exe と同じディレクトリに link.exe がありません: {linker}")
    return str(linker)


def variant_target_name(base_target):
    """shlib_variant を追加したターゲット定義の名前。"""
    return base_target + "-app"


def base_target_name(target):
    """variant_target_name の逆。OpenSSL が定義する元のターゲット名を返す。"""
    suffix = "-app"
    return target[:-len(suffix)] if target.endswith(suffix) else target


def variant_config_text(base_target):
    """元のターゲットを継承し、shlib_variant だけを追加するターゲット定義。

    Configure の --config で渡すため、OpenSSL 本体にはパッチを当てない。
    """
    return (
        "# build_openssl.py が生成する。直接編集しない。\n"
        "my %targets = (\n"
        f'    "{variant_target_name(base_target)}" => {{\n'
        f'        inherit_from  => [ "{base_target}" ],\n'
        f'        shlib_variant => "{SHLIB_VARIANT}",\n'
        "    },\n"
        ");\n"
    )


def configure_args(platform, machine, config, prefix, openssldir, variant_config, linker=None):
    """Configure に渡す引数。no-asm と no-pic は渡さない。

    variant_config は variant_config_text の内容を書いたファイルのパス。
    Windows では linker (MSVC の link.exe の絶対パス) を LD として渡す。
    """
    prefix_text = prefix.as_posix()
    openssldir_text = openssldir.as_posix()
    variant_config_text_path = variant_config.as_posix()
    if " " in prefix_text or " " in openssldir_text or " " in variant_config_text_path:
        raise BuildError("OpenSSL の prefix に空白を含められません。")
    selected = config or "RelWithDebInfo"
    if selected not in ("Debug", "Release", "RelWithDebInfo"):
        raise BuildError(f"未対応の CONFIG です: {selected}")
    args = [
        variant_target_name(target_name(platform, machine)),
        *FEATURE_ARGS,
        f"--config={variant_config_text_path}",
        f"--prefix={prefix_text}",
        "--libdir=lib",
        f"--openssldir={openssldir_text}",
    ]
    if selected == "Debug":
        args.append("--debug")
    if platform == "windows":
        if linker is None:
            raise BuildError("Windows では MSVC の link.exe の絶対パスが必要です。")
        # windows-makefile.tmpl は LD を二重引用符で囲むため、空白を含むパスを渡せる。
        args.append(f"LD={linker}")
        args.extend(WINDOWS_COMPILER_ARGS)
    return args


def runtime_lib_name(platform, base):
    """実行時に読み込まれる共有ライブラリのファイル名。Linux では SONAME と同じ。"""
    if platform == "windows":
        return f"lib{base}-{SHLIB_VERSION}-x64{SHLIB_VARIANT}.dll"
    if platform == "linux":
        return f"lib{base}{SHLIB_VARIANT}.so.{SHLIB_VERSION}"
    raise BuildError(f"未対応の OS です: {platform}")


def link_lib_name(platform, base):
    """リンク時に使うファイル名。利用側は LIBS += crypto_app のように指定する。"""
    if platform == "windows":
        return f"lib{base}{SHLIB_VARIANT}.lib"
    if platform == "linux":
        return f"lib{base}{SHLIB_VARIANT}.so"
    raise BuildError(f"未対応の OS です: {platform}")


def upstream_link_lib_name(platform, base):
    """OpenSSL のビルドが作るリンク用のファイル名。shlib_variant は反映されない。"""
    if platform == "windows":
        return f"lib{base}.lib"
    if platform == "linux":
        return f"lib{base}.so"
    raise BuildError(f"未対応の OS です: {platform}")


def static_archive_name(platform, base):
    """共有ビルドでも OpenSSL が作る静的アーカイブの名前。配置せず、検査だけに使う。"""
    if platform == "windows":
        return f"lib{base}_static.lib"
    if platform == "linux":
        return f"lib{base}.a"
    raise BuildError(f"未対応の OS です: {platform}")


def product_lib_names(platform):
    """prod/lib へ配置するファイル名。実行時用、リンク用の順。"""
    names = []
    for base in SHLIB_BASE_NAMES:
        names.append(runtime_lib_name(platform, base))
        names.append(link_lib_name(platform, base))
    return tuple(names)


def parse_perl_version(text):
    match = re.search(r"v?(\d+)\.(\d+)\.(\d+)", text)
    if match is None:
        raise BuildError(f"Perl の版を読めません: {text!r}")
    return tuple(int(part) for part in match.groups())


def require_perl(version, osname, platform):
    if version < MIN_PERL:
        found = ".".join(str(part) for part in version)
        raise BuildError(f"Perl 5.10 以上が必要です。検出: {found}")
    if platform == "windows" and osname != "MSWin32":
        raise BuildError(
            "Windows では osname が MSWin32 の Perl が必要です。"
            f" 検出: {osname}。"
            f" {PERL_COMMANDS['windows']} が Strawberry Perl を指しているか確認してください。"
            " see: https://github.com/openssl/openssl/blob/openssl-3.5.9/NOTES-WINDOWS.md"
        )


def find_perl(platform, which=None):
    """Configure を起動する Perl の絶対パス。"""
    if which is None:
        which = shutil.which
    name = PERL_COMMANDS.get(platform)
    if name is None:
        raise BuildError(f"未対応の OS です: {platform}")
    path = which(name)
    if path is not None:
        return path
    if platform == "windows":
        raise BuildError(
            f"{name} が PATH にありません。"
            f" devbin-win の Strawberry Perl を導入してください ({DEVBIN_WIN_URL})。"
            " Strawberry Perl の perl.exe を strawberry-perl.exe に改名し、"
            "perl\\bin を PATH の後段へ追加すると同じ状態になります。"
            " Git 付属の Perl は使えません。"
            " see: https://github.com/openssl/openssl/blob/openssl-3.5.9/NOTES-WINDOWS.md"
        )
    raise BuildError(
        f"{name} が PATH にありません。Perl 5.10 以上が必要です。"
        " see: https://github.com/openssl/openssl/blob/openssl-3.5.9/NOTES-PERL.md"
    )


def query_perl(perl):
    result = subprocess.run(
        [perl, "-MConfig", "-e", r"print $^V, qq{\n}, $Config{osname}, qq{\n}"],
        capture_output=True,
        text=True,
        encoding="utf-8",
        check=False,
    )
    if result.returncode != 0:
        detail = (result.stderr or "").strip()
        raise BuildError(f"perl を実行できません: {detail}")
    lines = [line.strip() for line in result.stdout.splitlines() if line.strip()]
    if len(lines) < 2:
        raise BuildError(f"perl の版と osname を読めません: {result.stdout!r}")
    return parse_perl_version(lines[0]), lines[1]


def parse_nasm_version(text):
    match = re.search(r"NASM version (\d+)\.(\d+)(?:\.(\d+))?", text)
    if match is None:
        raise BuildError(f"NASM の版を読めません: {text!r}")
    return tuple(int(part or 0) for part in match.groups())


def require_nasm(version):
    if version < MIN_NASM:
        found = ".".join(str(part) for part in version)
        needed = ".".join(str(part) for part in MIN_NASM)
        raise BuildError(
            f"NASM {needed} 以上が必要です。検出: {found}。"
            " see: https://github.com/openssl/openssl/blob/openssl-3.5.9/INSTALL.md"
        )


def query_nasm():
    if shutil.which("nasm") is None:
        raise BuildError(
            "nasm が PATH にありません。NASM 2.13.3 以上が必要です。"
            " see: https://www.nasm.us/"
            " see: https://github.com/openssl/openssl/blob/openssl-3.5.9/NOTES-WINDOWS.md"
        )
    result = subprocess.run(
        ["nasm", "-v"],
        capture_output=True,
        text=True,
        encoding="utf-8",
        check=False,
    )
    if result.returncode != 0:
        raise BuildError("nasm -v が失敗しました。")
    return parse_nasm_version(result.stdout or "")


def require_compiler(platform):
    if platform == "windows":
        missing = [name for name in ("cl", "nmake") if shutil.which(name) is None]
        if missing:
            raise BuildError(
                f"{', '.join(missing)} が PATH にありません。"
                " Visual Studio の開発者コマンドプロンプトで実行してください。"
                " see: https://github.com/openssl/openssl/blob/openssl-3.5.9/NOTES-WINDOWS.md"
            )
        return
    missing = [name for name in ("gcc", "make", "ar") if shutil.which(name) is None]
    if missing:
        raise BuildError(f"{', '.join(missing)} が PATH にありません。")


# Configure が読むコアモジュール。RPM の perl だけでは足りない。
# EL9 以降では FindBin も perl-interpreter から別パッケージ (perl-FindBin) に分かれている。
# see: https://github.com/openssl/openssl/blob/openssl-3.5.9/NOTES-PERL.md
REQUIRED_PERL_MODULES = ("FindBin", "IPC::Cmd", "Time::Piece")


def require_perl_modules(perl, run=None):
    """OpenSSL の Configure に必要な Perl モジュールがあることを確認する。"""
    if run is None:
        run = subprocess.run
    missing = []
    for name in REQUIRED_PERL_MODULES:
        result = run(
            [perl, f"-M{name}", "-e", "1"],
            capture_output=True,
            text=True,
            encoding="utf-8",
            check=False,
        )
        if result.returncode != 0:
            missing.append(name)
    if missing:
        raise BuildError(
            "Perl のコアモジュールがありません: " + ", ".join(missing) + "。"
            " RPM 系では perl-core を入れてください"
            " (個別に入れる場合は perl-IPC-Cmd、perl-Time-Piece、EL9 以降は perl-FindBin も)。"
            " see: https://github.com/openssl/openssl/blob/openssl-3.5.9/NOTES-PERL.md"
        )


def require_tools(platform):
    """ビルドに使うツールを確認し、Configure を起動する Perl の絶対パスを返す。"""
    require_compiler(platform)
    perl = find_perl(platform)
    version, osname = query_perl(perl)
    require_perl(version, osname, platform)
    require_perl_modules(perl)
    if platform == "windows":
        require_nasm(query_nasm())
    return perl


def list_packages(packages_dir):
    if not packages_dir.is_dir():
        return []
    return sorted(
        path.name
        for path in packages_dir.iterdir()
        if path.is_file() and PACKAGE_PATTERN.match(path.name)
    )


def select_package_name(names, known):
    if not names:
        raise BuildError(
            "OpenSSL のソース アーカイブが packages にありません。\n"
            "  ファイル名: openssl-3.5.9.tar.gz\n"
            "  取得例:\n"
            "    curl -L -o app/openssl/packages/openssl-3.5.9.tar.gz \\\n"
            f"      {UPSTREAM_URL}\n"
            "  packages/ には、対応表に載った tar.gz を 1 個置いてください。"
        )
    unknown = [name for name in names if name not in known]
    if unknown:
        raise BuildError(
            "対応表にないアーカイブです: "
            + ", ".join(unknown)
            + "。build_openssl.py の EXPECTED_SHA256 と packages/README.md を更新してください。"
        )

    def version(name):
        match = PACKAGE_PATTERN.match(name)
        return tuple(int(part) for part in match.groups())

    selected = max(names, key=version)
    rejected = [name for name in names if name != selected]
    return selected, rejected


def warn_extra_packages(selected, rejected):
    if not rejected:
        return
    lines = [
        "WARNING: packages に複数の OpenSSL アーカイブがあります。",
        f"  採用: {selected}",
    ]
    lines.extend(f"  未採用: {name}" for name in rejected)
    lines.append("  アーカイブは 1 個にしてください。")
    print("\n".join(lines), file=sys.stderr)


def sha256_file(path):
    digest = hashlib.sha256()
    with open(path, "rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def verify_sha256(path, expected):
    actual = sha256_file(path)
    if actual != expected:
        raise BuildError(
            f"SHA-256 が一致しません: {path.name}\n"
            f"  期待: {expected}\n"
            f"  実際: {actual}"
        )
    return actual


def read_text(path):
    return path.read_text(encoding="utf-8")


def write_text(path, text):
    path.parent.mkdir(parents=True, exist_ok=True)
    if not text.endswith("\n"):
        text += "\n"
    path.write_text(text, encoding="utf-8", newline="\n")


def script_digest():
    return hashlib.sha256(Path(__file__).read_bytes()).hexdigest()


def build_identity(package_name, digest, args):
    return (
        f"package={package_name}\n"
        f"sha256={digest}\n"
        f"args={' '.join(args)}\n"
        f"script={script_digest()}\n"
    )


def outputs_ready(app, platform):
    lib_dir = app / "prod" / "lib"
    include_dir = app / "prod" / "include" / "openssl"
    if any(not (lib_dir / name).is_file() for name in product_lib_names(platform)):
        return False
    if any((lib_dir / name).exists() for name in LEGACY_PRODUCT_LIB_NAMES):
        return False
    for header in ("ssl.h", "configuration.h", "opensslv.h"):
        if not (include_dir / header).is_file():
            return False
    if not app.joinpath(*LICENSE_RELATIVE).is_file():
        return False
    return True


def find_source_root(src_dir):
    direct = src_dir / "Configure"
    if direct.is_file():
        return src_dir
    found = [path.parent for path in src_dir.glob("*/Configure") if path.is_file()]
    if len(found) != 1:
        raise BuildError(f"展開先から Configure を特定できません: {src_dir}")
    return found[0]


def source_root_from_stamp(app):
    stamp = app / "gen" / "source-root.txt"
    if not stamp.is_file():
        return None
    relative = read_text(stamp).strip()
    if not relative:
        return None
    root = app / relative
    if not (root / "Configure").is_file():
        return None
    return root


def ensure_include_gitignore(app):
    path = app / "prod" / "include" / ".gitignore"
    if path.is_file() and read_text(path) == INCLUDE_GITIGNORE:
        return
    write_text(path, INCLUDE_GITIGNORE)


def remove_product_libs(app):
    """prod/lib から、現在の名前と以前の名前の OpenSSL の成果物を削除する。"""
    lib_dir = app / "prod" / "lib"
    if not lib_dir.is_dir():
        return
    prefixes = tuple(f"lib{base}" for base in SHLIB_BASE_NAMES)
    for path in list(lib_dir.iterdir()):
        if not (path.is_file() or path.is_symlink()):
            continue
        stem = path.name.split(".", 1)[0]
        is_current = any(
            stem == prefix or stem.startswith((prefix + SHLIB_VARIANT, prefix + "-"))
            for prefix in prefixes
        )
        if is_current or path.name in LEGACY_PRODUCT_LIB_NAMES:
            path.unlink()


def remove_products(app):
    remove_product_libs(app)
    headers = app / "prod" / "include" / "openssl"
    if headers.exists():
        shutil.rmtree(headers)
    license_path = app.joinpath(*LICENSE_RELATIVE)
    if license_path.is_file():
        license_path.unlink()


def remove_gen_except_lock(gen_dir):
    if not gen_dir.exists():
        return
    for child in list(gen_dir.iterdir()):
        if child.name == ".build.lock":
            continue
        if child.is_dir() and not child.is_symlink():
            shutil.rmtree(child)
        else:
            child.unlink()


@contextmanager
def build_lock(lock_path):
    """展開から配置までをプロセス間で直列化する。"""
    # see: https://docs.python.org/3/library/fcntl.html#fcntl.flock
    # see: https://docs.python.org/3/library/msvcrt.html#msvcrt.locking
    lock_path.parent.mkdir(parents=True, exist_ok=True)
    with open(lock_path, "a+b") as handle:
        if os.fstat(handle.fileno()).st_size == 0:
            handle.write(b"0")
            handle.flush()
        if os.name == "nt":
            import msvcrt
            deadline = time.monotonic() + 60
            while True:
                handle.seek(0)
                try:
                    msvcrt.locking(handle.fileno(), msvcrt.LK_NBLCK, 1)
                    break
                except OSError as exc:
                    winerror = getattr(exc, "winerror", None)
                    # 5: ACCESS_DENIED, 32: SHARING_VIOLATION, 33: LOCK_VIOLATION
                    if winerror not in (5, 32, 33) and exc.errno not in (
                        errno.EACCES, errno.EAGAIN, errno.EDEADLK,
                    ):
                        raise
                    if time.monotonic() >= deadline:
                        raise BuildError("OpenSSL のビルドロックを取得できません。") from exc
                    time.sleep(0.05)
        else:
            import fcntl
            fcntl.flock(handle.fileno(), fcntl.LOCK_EX)
        try:
            yield
        finally:
            if os.name == "nt":
                handle.seek(0)
                msvcrt.locking(handle.fileno(), msvcrt.LK_UNLCK, 1)
            else:
                fcntl.flock(handle.fileno(), fcntl.LOCK_UN)


def clean_app(app):
    gen_dir = app / "gen"
    gen_dir.mkdir(parents=True, exist_ok=True)
    with build_lock(gen_dir / ".build.lock"):
        remove_products(app)
        remove_gen_except_lock(gen_dir)
    lock_path = gen_dir / ".build.lock"
    try:
        lock_path.unlink()
    except FileNotFoundError:
        pass
    try:
        gen_dir.rmdir()
    except OSError:
        pass


def ensure_source(app, tar_path, digest):
    gen_dir = app / "gen"
    gen_dir.mkdir(parents=True, exist_ok=True)
    stamp = gen_dir / "extract.stamp"
    expected = f"sha256={digest}\n"
    root = source_root_from_stamp(app)
    if stamp.is_file() and read_text(stamp) == expected and root is not None:
        return root
    src_dir = gen_dir / "src"
    install_dir = gen_dir / "install"
    if src_dir.exists():
        shutil.rmtree(src_dir)
    if install_dir.exists():
        shutil.rmtree(install_dir)
    src_dir.mkdir(parents=True)
    print(f"INFO: アーカイブを展開しています: {tar_path.name}", file=sys.stderr)
    try:
        with tarfile.open(tar_path, "r:gz") as archive:
            archive.extractall(src_dir, filter="data")
    except (OSError, tarfile.TarError) as exc:
        raise BuildError(f"アーカイブを展開できません: {exc}") from exc
    root = find_source_root(src_dir)
    write_text(gen_dir / "source-root.txt", root.relative_to(app).as_posix())
    write_text(stamp, expected)
    configure_stamp = gen_dir / "configure.stamp"
    if configure_stamp.exists():
        configure_stamp.unlink()
    return root


def drop_unsupported_enables(args, log_bytes):
    """Configure が拒否した enable-* だけを外す。shared などは残す。"""
    if b"Unsupported options" not in log_bytes:
        return None
    dropped = [
        arg for arg in args
        if arg.startswith("enable-") and arg.encode("ascii") in log_bytes
    ]
    if not dropped:
        return None
    return [arg for arg in args if arg not in dropped]


def run_logged(cmd, cwd, env, log_path, keep=False, append=False, inherit_fds=False):
    """コマンドを実行し、出力をログと標準出力へ書く。

    inherit_fds は、親 make のジョブ サーバーの fd を子 make へ引き継ぐ場合に True にする。
    """
    log_path.parent.mkdir(parents=True, exist_ok=True)
    print("INFO: " + " ".join(cmd), file=sys.stderr)
    chunks = []
    with open(log_path, "ab" if append else "wb") as log:
        proc = subprocess.Popen(
            cmd,
            cwd=cwd,
            env=env,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            close_fds=not inherit_fds,
        )
        assert proc.stdout is not None
        for chunk in iter(lambda: proc.stdout.read(8192), b""):
            if keep:
                chunks.append(chunk)
            log.write(chunk)
            sys.stdout.buffer.write(chunk)
            sys.stdout.buffer.flush()
        code = proc.wait()
    return code, b"".join(chunks)


def configure_source(source, args, perl):
    # source は gen/src/<version>。ログは gen/configure.log に置く。
    log_path = source.parent.parent / "configure.log"
    env = child_env("configure")
    code, output = run_logged(
        [perl, "Configure", *args], source, env, log_path, keep=True,
    )
    if code == 0:
        return
    reduced = drop_unsupported_enables(args, output)
    if reduced is None:
        raise BuildError(
            f"Configure が失敗しました (exit {code})。ログ: {log_path}"
        )
    print(
        "WARNING: Configure が enable 指定を拒否したため、既定の有効化のまま再実行します。",
        file=sys.stderr,
    )
    code, _output = run_logged(
        [perl, "Configure", *reduced], source, env, log_path, keep=True,
    )
    if code != 0:
        raise BuildError(
            f"Configure が失敗しました (exit {code})。ログ: {log_path}"
        )


def makefile_variable(data, name):
    """生成された Makefile から、行頭の NAME= の値を返す。無ければ None。"""
    match = re.search(
        rb"^" + re.escape(name.encode("ascii")) + rb"=(.*)$", data, re.MULTILINE,
    )
    if match is None:
        return None
    return match.group(1)


def assert_pic(source):
    """ライブラリのオブジェクトが PIC でコンパイルされることを、生成された Makefile で確認する。"""
    makefile = source / "Makefile"
    data = makefile.read_bytes() if makefile.is_file() else b""
    cflags = (makefile_variable(data, "LIB_CFLAGS") or b"").split()
    cppflags = (makefile_variable(data, "LIB_CPPFLAGS") or b"").split()
    if not any(flag in (b"-fPIC", b"-fpic") for flag in cflags) or b"-DOPENSSL_PIC" not in cppflags:
        raise BuildError(
            "Makefile の LIB_CFLAGS に -fPIC、または LIB_CPPFLAGS に -DOPENSSL_PIC がありません。"
            " 共有ライブラリをビルドできません。"
        )


def defined_macros(data):
    return set(_DEFINE_RE.findall(data))


def assert_config_header(path):
    defined = defined_macros(path.read_bytes())
    disabled = [name.decode("ascii") for name in DISABLED_MACROS if name in defined]
    if disabled:
        raise BuildError("期待した機能が無効です: " + ", ".join(disabled))
    missing = [name.decode("ascii") for name in REQUIRED_MACROS if name not in defined]
    if missing:
        raise BuildError("期待した機能の無効化が反映されていません: " + ", ".join(missing))


def assert_version_header(path, version):
    token = f'"{version}"'.encode("ascii")
    if token not in path.read_bytes():
        raise BuildError(f"{path.name} に {version} がありません。")


def assert_shared_variant(path, platform, base):
    """共有ライブラリに shlib_variant が反映されていることを、ファイルの内容で確認する。

    Linux では、SONAME の文字列と OPENSSL_APP_ のシンボル バージョンがあり、
    標準の OPENSSL_ のシンボル バージョンが無いことを見る。
    Windows の DLL にはシンボル バージョンが無いため、DLL が自分の名前を持つことを見る。
    """
    data = path.read_bytes()
    name = runtime_lib_name(platform, base).encode("ascii")
    if name not in data:
        raise BuildError(f"{path.name} に共有ライブラリの名前 {name.decode('ascii')} がありません。")
    if platform != "linux":
        return
    if b"\0" + VARIANT_SYMBOL_VERSION + b"\0" not in data:
        raise BuildError(
            f"{path.name} にシンボル バージョン {VARIANT_SYMBOL_VERSION.decode('ascii')} がありません。"
        )
    if b"\0" + DEFAULT_SYMBOL_VERSION + b"\0" in data:
        raise BuildError(
            f"{path.name} に標準のシンボル バージョン {DEFAULT_SYMBOL_VERSION.decode('ascii')} があります。"
            " OS の OpenSSL と区別できません。"
        )


def assert_import_lib(path, platform, base):
    """Windows の import library が、shlib_variant の付いた DLL を参照することを確認する。"""
    if platform != "windows":
        return
    name = runtime_lib_name(platform, base).encode("ascii")
    if name not in path.read_bytes():
        raise BuildError(f"{path.name} が {name.decode('ascii')} を参照していません。")


def assert_asm(lib_path, target):
    """静的アーカイブのメンバー名で、アセンブラー実装が有効であることを確認する。

    共有ビルドでも OpenSSL は同じオブジェクトから静的アーカイブを作るため、それを調べる。
    """
    markers = ASM_MARKERS[base_target_name(target)]
    if target.startswith("VC-"):
        if shutil.which("lib") is None:
            print(
                "WARNING: lib.exe がないため、アーカイブ内のアセンブラー成果物は未確認です。",
                file=sys.stderr,
            )
            return
        result = subprocess.run(
            ["lib", "/nologo", "/list", str(lib_path)],
            capture_output=True,
            check=False,
        )
        blob = result.stdout + result.stderr
        if result.returncode != 0:
            raise BuildError(f"lib.exe が {lib_path.name} を読めません。")
    else:
        result = subprocess.run(
            ["ar", "t", str(lib_path)],
            capture_output=True,
            check=False,
        )
        blob = result.stdout
        if result.returncode != 0:
            raise BuildError(f"ar が {lib_path.name} を読めません。")
    if not any(marker in blob for marker in markers):
        raise BuildError(f"アセンブラー成果物が見つかりません: {lib_path.name}")


def find_file(candidates, label):
    for path in candidates:
        if path.is_file():
            return path
    rendered = "\n".join(str(path) for path in candidates)
    raise BuildError(f"{label} が見つかりません:\n{rendered}")


def atomic_copy(src, dest):
    dest.parent.mkdir(parents=True, exist_ok=True)
    temporary = dest.with_name(dest.name + ".tmp")
    shutil.copy2(src, temporary)
    os.replace(temporary, dest)


def copy_tree_files(src_dir, dest_dir):
    if dest_dir.exists():
        shutil.rmtree(dest_dir)
    dest_dir.mkdir(parents=True)
    for path in src_dir.rglob("*"):
        relative = path.relative_to(src_dir)
        target = dest_dir / relative
        if path.is_dir():
            target.mkdir(parents=True, exist_ok=True)
            continue
        if not path.is_file():
            raise BuildError(f"通常ファイルではありません: {path}")
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(path, target)


def copy_pdbs(source, dest_lib, platform):
    """Windows の DLL と同じ名前の PDB を、DLL と同じ場所へ配置する。

    静的アーカイブ用の ossl_static.pdb や providers/legacy.pdb は配置しない。
    see: https://github.com/openssl/openssl/blob/openssl-3.5.9/Configurations/10-main.conf
    """
    if platform != "windows":
        return
    for base in SHLIB_BASE_NAMES:
        path = (source / runtime_lib_name(platform, base)).with_suffix(".pdb")
        if path.is_file():
            atomic_copy(path, dest_lib / path.name)


def place_link_lib(source, dest_lib, platform, base):
    """リンク用のファイルを、shlib_variant の付いた名前で配置する。

    OpenSSL のビルドが作るリンク用のファイル名 (libcrypto.so、libcrypto.lib) には
    shlib_variant が付かない。利用側が LIBS += crypto_app で解決できる名前にする。
    Linux は実行時用のファイルへの相対シンボリック リンクを作る。
    Windows は import library をコピーする。import library が参照する DLL 名は、
    ファイル名を変えても変わらない。
    """
    dest = dest_lib / link_lib_name(platform, base)
    if platform == "linux":
        temporary = dest.with_name(dest.name + ".tmp")
        if temporary.is_symlink() or temporary.exists():
            temporary.unlink()
        os.symlink(runtime_lib_name(platform, base), temporary)
        os.replace(temporary, dest)
        return
    upstream = find_file(
        [source / upstream_link_lib_name(platform, base)],
        upstream_link_lib_name(platform, base),
    )
    assert_import_lib(upstream, platform, base)
    atomic_copy(upstream, dest)


def build_and_install(source, platform):
    log_path = source.parent.parent / "build.log"
    if platform == "windows":
        # nmake は並列ビルドができないため、Windows のビルドは時間がかかる。
        env = child_env("nmake")
        commands = (["nmake"], ["nmake", "install_dev"])
    else:
        env = child_env("make")
        commands = (["make"], ["make", "install_dev"])
    for index, cmd in enumerate(commands):
        code, _output = run_logged(
            cmd, source, env, log_path,
            append=index > 0,
            inherit_fds=platform != "windows",
        )
        if code != 0:
            raise BuildError(
                f"コマンドが失敗しました (exit {code}): {' '.join(cmd)}\nログ: {log_path}"
            )


def needs_configure(app, source, args):
    makefile = source / "Makefile"
    stamp = app / "gen" / "configure.stamp"
    expected = "args=" + " ".join(args) + "\n"
    if not makefile.is_file() or not stamp.is_file():
        return True
    return read_text(stamp) != expected


def build_app(app, config):
    platform = host_platform()
    packages_dir = app / "packages"
    names = list_packages(packages_dir)
    selected, rejected = select_package_name(names, EXPECTED_SHA256)
    warn_extra_packages(selected, rejected)
    tar_path = packages_dir / selected
    digest = verify_sha256(tar_path, EXPECTED_SHA256[selected])
    prefix = (app / "gen" / "install").resolve()
    openssldir = prefix / "ssl"
    linker = find_msvc_linker() if platform == "windows" else None
    variant_config = (app / "gen" / "shlib-variant.conf").resolve()
    args = configure_args(
        platform, platform_mod.machine(), config, prefix, openssldir, variant_config, linker,
    )
    identity = build_identity(selected, digest, args)
    stamp_path = app / "gen" / "build.stamp"
    if stamp_path.is_file() and read_text(stamp_path) == identity and outputs_ready(app, platform):
        print("INFO: OpenSSL のビルド成果物は最新です。", file=sys.stderr)
        return

    perl = require_tools(platform)
    ensure_include_gitignore(app)
    gen_dir = app / "gen"
    gen_dir.mkdir(parents=True, exist_ok=True)
    with build_lock(gen_dir / ".build.lock"):
        if stamp_path.is_file() and read_text(stamp_path) == identity and outputs_ready(app, platform):
            print("INFO: OpenSSL のビルド成果物は最新です。", file=sys.stderr)
            return
        source = ensure_source(app, tar_path, digest)
        if needs_configure(app, source, args):
            if (source / "Makefile").is_file():
                print("INFO: Configure 引数が変わったため、展開からやり直します。", file=sys.stderr)
                extract_stamp = gen_dir / "extract.stamp"
                if extract_stamp.exists():
                    extract_stamp.unlink()
                source = ensure_source(app, tar_path, digest)
            print(f"INFO: OpenSSL {OPENSSL_VERSION} を Configure します。", file=sys.stderr)
            write_text(
                variant_config,
                variant_config_text(target_name(platform, platform_mod.machine())),
            )
            configure_source(source, args, perl)
            write_text(gen_dir / "configure.stamp", "args=" + " ".join(args))
        if platform == "linux":
            assert_pic(source)
        generated = source / "include" / "openssl" / "configuration.h"
        if generated.is_file():
            assert_config_header(generated)
        if stamp_path.exists():
            stamp_path.unlink()
        print(f"INFO: OpenSSL {OPENSSL_VERSION} をビルドします。", file=sys.stderr)
        build_and_install(source, platform)
        runtime_libs = []
        for base in SHLIB_BASE_NAMES:
            name = runtime_lib_name(platform, base)
            path = find_file([source / name], name)
            assert_shared_variant(path, platform, base)
            runtime_libs.append((base, path))
        header_dir = find_file(
            [
                prefix / "include" / "openssl" / "ssl.h",
                source / "include" / "openssl" / "ssl.h",
            ],
            "include/openssl/ssl.h",
        ).parent
        static_archive = static_archive_name(platform, SHLIB_BASE_NAMES[0])
        assert_asm(find_file([source / static_archive], static_archive), args[0])
        dest_lib = app / "prod" / "lib"
        remove_product_libs(app)
        for base, path in runtime_libs:
            atomic_copy(path, dest_lib / path.name)
            place_link_lib(source, dest_lib, platform, base)
        copy_tree_files(header_dir, app / "prod" / "include" / "openssl")
        copy_pdbs(source, dest_lib, platform)
        atomic_copy(
            find_file([source / LICENSE_NAME], LICENSE_NAME),
            app.joinpath(*LICENSE_RELATIVE),
        )
        assert_config_header(app / "prod" / "include" / "openssl" / "configuration.h")
        assert_version_header(
            app / "prod" / "include" / "openssl" / "opensslv.h",
            OPENSSL_VERSION,
        )
        write_text(stamp_path, identity)
        print(
            "INFO: 共有ライブラリを配置しました: "
            + ", ".join(str(dest_lib / name) for name in product_lib_names(platform)),
            file=sys.stderr,
        )


def parse_args(argv):
    parser = argparse.ArgumentParser(
        description="OpenSSL の共有ライブラリをビルドして prod へ配置します。",
    )
    parser.add_argument("--app-dir", required=True)
    parser.add_argument("--config", default="RelWithDebInfo")
    parser.add_argument("--clean", action="store_true")
    return parser.parse_args(argv)


def main(argv=None):
    configure_stdio()
    try:
        args = parse_args(sys.argv[1:] if argv is None else argv)
        app = Path(args.app_dir).resolve()
        if args.clean:
            clean_app(app)
            return 0
        build_app(app, args.config)
        return 0
    except BuildError as exc:
        print(f"ERROR: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
