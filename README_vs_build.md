# Building QuantLibXL from Source Code (Visual Studio)

This document explains how to build the QuantLibXL Excel add-in and its
prerequisites from source code using the hand-maintained Visual Studio
solution files.

> **Last verified:** built successfully with VS 2026 (v145 toolset), x64,
> Release (dynamic runtime), producing
> `QuantLibXL\xll\QuantLibXL-v145-x64-mt-1_23_0.xll`.

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

Boost is managed via conan. Boost 1.83.0 for x64/Release/dynamic-runtime
(`compiler.runtime=dynamic`) must be present in the local conan cache.

To check whether it is already cached:

```
conan list boost/1.83.0
```

If it is not cached, install it:

```
conan install . --output-folder=conan --build=missing -s build_type=Release
```

Once the package is in the cache, find its directory:

```
conan cache path boost/1.83.0:<package-id>
```

where `<package-id>` is the hash shown by `conan list boost/1.83.0:*`.

Then open `boost.props` in the repository root and set `BoostIncludeDir`
and `BoostLibDir` to point at that directory, e.g.:

```xml
<BoostIncludeDir>C:\Users\username\.conan2\p\boostXXXXXXXXXXXXX\p\include</BoostIncludeDir>
<BoostLibDir>C:\Users\username\.conan2\p\boostXXXXXXXXXXXXX\p\lib</BoostLibDir>
```

`boost.props` is imported by every project in the solution via
`QuantLib\QuantLib.props`. It also sets `<LanguageStandard>stdcpp17</LanguageStandard>`
globally, which is required by the QuantLib 1.42 headers.

> **Note:** Only the dynamic-runtime (`/MD`) build has been tested.
> The static-runtime configurations (`Release (static runtime)`) would
> require a separate conan install with `compiler.runtime=static` and
> corresponding static Boost libraries. This has not been set up yet.

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
| Dynamic-runtime XLL (tested, recommended) | **Release** | **x64** |
| Static-runtime XLL (not yet set up) | **Release (static runtime)** | **x64** |

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

---

## 6 Loading the Add-in in Excel

1. Open Excel.
2. Go to **File > Options > Add-ins**.
3. At the bottom, set **Manage** to **Excel Add-ins** and click **Go**.
4. Click **Browse** and navigate to the `.xll` file produced above.
5. Click **OK**.

Alternatively, drag and drop the `.xll` file onto an open Excel workbook.

