# Building QuantLibXL from Source Code (Visual Studio)

This document explains how to build the QuantLibXL Excel add-in and its
prerequisites from source code using the hand-maintained Visual Studio
solution files.

> **Last verified:** all 8 configurations (4 runtime variants × basic + full)
> built successfully with VS 2026 (v145 toolset), x64, producing all four
> XLLs in `QuantLibXL\xll\`. See sections 2.2 and 5 for full details.

---

## 1 Choose a Build - Basic or Full

Two solution files are provided:

| Build | Solution file | What it does |
|---|---|---|
| Basic | `QuantLibXL\QuantLibXL_basic.sln` | Compiles pre-existing auto-generated source files. Does not require Python or gensrc. |
| Full | `QuantLibXL\QuantLibXL_full.sln` | Runs gensrc first to regenerate all auto-generated source files, then compiles. Requires Python 3 and nmake. |

Use the Basic build for normal compilation. Use the Full build only if you
have changed the gensrc metadata (the XML files under
`ObjectHandler\gensrc\metadata` or `QuantLibAddin\gensrc\metadata`).

---

## 2 Prerequisites

### 2.1 Visual Studio

VS 2026 (v145 toolset) or VS 2022 (v143 toolset) is required. The
"Desktop development with C++" workload must be installed. The platform
toolset is selected automatically from `QuantLib\QuantLib.props` based on
the `VisualStudioVersion` environment variable.

For the Full build, the **"Desktop development with C++"** workload must
include the **C++ CMake tools** component, which provides `nmake.exe` on
the PATH inside a Developer Command Prompt. Alternatively, nmake can be
found at:

```
C:\Program Files\Microsoft Visual Studio\<version>\Professional\VC\Tools\MSVC\<toolset>\bin\Hostx64\x64\nmake.exe
```

### 2.2 Boost

`boost.props` in the repository root controls Boost include and library
paths. It is configuration-aware: each of the four runtime/build-type
combinations points to a different Boost library directory. The include
directory (headers) is shared across all configurations.

#### Dynamic-runtime configurations (Release and Debug, `/MD`/`/MDd`)

These use a conan-cached Boost 1.83.0 build. The Release dynamic build
has already been provisioned; the same conan package also contains the
Debug dynamic (`-mt-gd-`) libraries.

To check whether the package is already cached:

```
conan list boost/1.83.0
```

If it is not cached, install it:

```
conan install . --output-folder=conan --build=missing -s build_type=Release
```

Once cached, find its directory:

```
conan cache path boost/1.83.0:<package-id>
```

where `<package-id>` is the hash shown by `conan list boost/1.83.0:*`.

#### Static-runtime configurations (Release and Debug static, `/MT`/`/MTd`)

Conan cannot download the static-runtime Boost packages when the corporate
firewall blocks the package server. Instead, use a locally pre-built Boost
snapshot. The snapshot at `C:\erik\junk\boost\boost_1_83_0` contains:

- `stage\lib` — Debug dynamic (`-mt-gd-`) and static-runtime
  (`libboost_*-mt-s-*`, `libboost_*-mt-sgd-*`) libraries built with vc143
- `stage-mt\lib` — Static-runtime libraries with full vc143 decorated names
  (`libboost_*-vc143-mt-s-*`, `libboost_*-vc143-mt-sgd-*`)

#### Configuring boost.props

Open `boost.props` in the repository root. It contains four
configuration-conditional `BoostLibDir` blocks — one per build type — plus
a single shared `BoostIncludeDir`. Update each path to match the actual
locations on the machine being used:

```xml
<!-- shared headers -->
<BoostIncludeDir>C:\Users\username\.conan2\p\boostXXXXXXXXXXXXX\p\include</BoostIncludeDir>

<!-- Release  (/MD)  — conan cache -->
<!-- Debug    (/MDd) — local snapshot stage\lib  -->
<!-- Release (static runtime) (/MT)  — local snapshot stage-mt\lib -->
<!-- Debug   (static runtime) (/MTd) — local snapshot stage-mt\lib -->
```

`boost.props` is imported by every project in the solution via
`QuantLib\QuantLib.props`. It also sets `<LanguageStandard>stdcpp17</LanguageStandard>`
globally, which is required by the QuantLib headers.

### 2.3 Python 3 (Full build only)

The Full build runs gensrc, a Python 3 script. Python must be installed and
`python` must be on the PATH (i.e. `python --version` works from a command
prompt). If `.py` files are not associated with the Python executable, edit
`ObjectHandler\gensrc\Makefile.vc` and `QuantLibAddin\gensrc\Makefile.vc`
and set the `PYTHON` variable to the full path of the Python executable:

```
PYTHON=C:\Program Files\Python312\python.exe
```

---

## 3 Repository Layout

The solution files use relative paths to locate dependent projects. The
expected layout after cloning this repository is:

```
QuantLibAddin\
  boost.props           # Boost location settings — edit before building
  conanfile.txt         # conan package descriptor (boost/1.83.0)
  gensrc\               # code-generation framework
  log4cxx\              # logging library
  ObjectHandler\        # object repository
  QuantLib\             # QuantLib C++ analytics library (git submodule)
  QuantLibAddin\        # QuantLib C++ wrapper
  QuantLibXL\           # Excel XLL
```

---

## 4 Build Steps

### Step 1 — Edit `boost.props`

Open `boost.props` in the repository root and update `BoostIncludeDir` and
`BoostLibDir` to the conan-cached Boost package on your machine, as
described in section 2.2.

### Step 2 — Open the solution

- **Basic build**: open `QuantLibXL\QuantLibXL_basic.sln` in Visual Studio.
- **Full build**: open `QuantLibXL\QuantLibXL_full.sln` in Visual Studio.

### Step 3 — Select configuration and platform

In the Visual Studio toolbar select:

| Goal | Configuration | Platform |
|---|---|---|
| Dynamic-runtime release XLL | **Release** | **x64** |
| Dynamic-runtime debug XLL | **Debug** | **x64** |
| Static-runtime release XLL | **Release (static runtime)** | **x64** |
| Static-runtime debug XLL | **Debug (static runtime)** | **x64** |

Always build for **x64**, not Win32.

### Step 4 — Build

Choose **Build > Build Solution** (`Ctrl+Shift+B`).

For the Full build, the gensrc projects (`ohgensrc`, `qlgensrc`) run first,
invoking `nmake` and Python to regenerate source files. Compilation of all
other projects follows automatically in dependency order:
`QuantLib` → `xlsdk`, `apr`, `aprutil`, `log4cxx`, `ohxllib` →
`QuantLibObjects` → `QuantLibXLStatic`.

---

## 5 Output

On a successful build the XLL is written to `QuantLibXL\xll\`. The filename
encodes the toolset, platform, configuration and version:

| Configuration | Platform | Output filename |
|---|---|---|
| Release | x64 | `QuantLibXL-v145-x64-mt-1_23_0.xll` |
| Release (static runtime) | x64 | `QuantLibXL-v145-x64-mt-s-1_23_0.xll` |
| Debug | x64 | `QuantLibXL-v145-x64-mt-gd-1_23_0.xll` |
| Debug (static runtime) | x64 | `QuantLibXL-v145-x64-mt-sgd-1_23_0.xll` |

The toolset tag (`v145`, `v143`, …) is determined automatically from the
Visual Studio version used to open the solution.

The basic and full builds share the same output filenames and overwrite each
other. Build full only when gensrc metadata has changed; use basic otherwise.

---

## 5.1 Command-line builds (MSBuild)

To build from the command line without opening Visual Studio, set
`VisualStudioVersion` so that `QuantLib.props` selects the correct toolset,
and add `nmake.exe` to the PATH for Full builds:

```powershell
$env:VisualStudioVersion = "18.0"
$env:PATH = "C:\Program Files\Microsoft Visual Studio\18\Professional\VC\Tools\MSVC\14.50.35717\bin\Hostx64\x64;" + $env:PATH
$msbuild = "C:\Program Files\Microsoft Visual Studio\18\Professional\MSBuild\Current\Bin\MSBuild.exe"

# example: Release (static runtime), basic build
&$msbuild "C:\erik\repos\QuantLibAddin\QuantLibXL\QuantLibXL_basic.sln" `
    /p:Configuration="Release (static runtime)" /p:Platform=x64 /m /nologo
```

Configuration names with spaces (e.g. `"Release (static runtime)"`) must be
quoted. The `/m` flag enables parallel compilation. Expected build times on
this machine are roughly:

| Configuration | Basic | Full |
|---|---|---|
| Release / Debug | ~30 s (incremental) | ~1 min (incremental) |
| Release (static) / Debug (static) | ~20 min (clean) | ~40 min (clean) |

Static-runtime clean builds are slow because QuantLib itself must be
recompiled from scratch for the new runtime library setting.

---

## 6 Loading the Add-in in Excel

1. Open Excel.
2. Go to **File > Options > Add-ins**.
3. At the bottom, set **Manage** to **Excel Add-ins** and click **Go**.
4. Click **Browse** and navigate to the `.xll` file produced above.
5. Click **OK**.

Alternatively, drag and drop the `.xll` file onto an open Excel workbook.

