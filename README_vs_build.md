# Building QuantLibXL from Source Code (Visual Studio)

This document explains how to build the QuantLibXL Excel add-in and its
prerequisites from source code using the hand-maintained Visual Studio
solution files.

---

## 1 Choose a Build - Basic or Full

Two solution files are provided:

| Build | Solution file | What it does |
|---|---|---|
| Basic | `QuantLibXL\QuantLibXL_basic.sln` | Compiles pre-existing auto-generated source files. Does not require Python or gensrc. |
| Full | `QuantLibXL\QuantLibXL_full.sln` | Runs gensrc first to regenerate all auto-generated source files, then compiles. Requires Python 3 and gensrc. |

For most purposes the Basic build is sufficient. Use the Full build only
if you have changed the gensrc metadata (the XML files under
`ObjectHandler\gensrc\metadata` or `QuantLibAddin\gensrc\metadata`).

---

## 2 Prerequisites

### 2.1 Visual Studio

Visual Studio 2022 (v145 toolset) is required. The "Desktop development with
C++" workload must be installed.

### 2.2 Boost

QuantLibXL depends on Boost. Boost is managed via conan in this repository -
the same conan setup used by the ORE project.

#### Dynamic-runtime build (Release / Debug)

If you have already run a conan install for ORE then the required Boost
packages are already on your machine. Otherwise, run conan to install them:

```
cd C:\path\to\QuantLibAddin
conan install . --output-folder=conan --build=missing -s build_type=Release
```

Once Boost is installed you need to tell the Visual Studio projects where to
find it. This is done by editing the file `boost.props` in the root of the
repository. Open it in a text editor and update the following paths inside the
`BoostConan` property group to match the conan-installed Boost on your machine:

```xml
<BoostPackageDir>C:\Users\username\.conan2\p\boostXXXXXXXXXXXXX\p</BoostPackageDir>
<WinSDKUmLibDir>C:\Program Files (x86)\Windows Kits\10\Lib\10.0.XXXXX.X\um\x64</WinSDKUmLibDir>
<WinSDKUcrtLibDir>C:\Program Files (x86)\Windows Kits\10\Lib\10.0.XXXXX.X\ucrt\x64</WinSDKUcrtLibDir>
<VCLibDir>C:\Program Files\Microsoft Visual Studio\2022\...\VC\Tools\MSVC\XX.XX\lib\x64</VCLibDir>
```

Also update the `BoostSuffix` to match the version and toolset of the Boost
libraries in your conan cache, e.g.:

```xml
<BoostSuffix>vc143-mt-x64-1_83</BoostSuffix>
```

The `BoostIncludeDir` and `BoostLibDir` properties are derived automatically
from `BoostPackageDir` and do not need to be set separately.

#### Static-runtime build (Release (static runtime) / Debug (static runtime))

The static-runtime configurations link Boost with `/MT` (no MSVC runtime DLL
dependency). Conan must be run a second time to obtain Boost libraries built
with the static runtime:

```powershell
cd C:\path\to\QuantLibAddin
conan install . --output-folder=conan-static --build=missing `
    -s build_type=Release `
    -s compiler.runtime=static `
    -s compiler.runtime_type=Release
```

After this install, look up the resulting Boost package directory in the conan
cache (it will be a different hash from the dynamic one). Then edit `boost.props`
and update the `BoostConanStatic` property group:

```xml
<BoostStaticPackageDir>C:\Users\username\.conan2\p\boostYYYYYYYYYYYYY\p</BoostStaticPackageDir>
<BoostStaticReleaseSuffix>vc143-mt-s-x64-1_83</BoostStaticReleaseSuffix>
<BoostStaticDebugSuffix>vc143-mt-sgd-x64-1_83</BoostStaticDebugSuffix>
```

Tip: list the `.lib` files inside `$(BoostStaticPackageDir)\lib` to confirm the
exact suffix used in the filenames (look for `-s-` for release and `-sgd-` for
debug).

> **Why two Boost installs?**  MSVC requires that every `.obj` and `.lib` file
> in a translation unit is compiled with the **same** CRT selection (`/MD` vs
> `/MT`).  Boost ships pre-compiled `.lib` files; the ones from the default
> conan install are `/MD`.  Mixing `/MT` objects with `/MD` Boost libs causes
> linker error LNK2038 ("mismatch detected for RuntimeLibrary").  The second
> conan install produces a fully `/MT`-compiled set of Boost libraries that are
> ABI-compatible with the static-runtime XLL build.

### 2.3 Python 3 (Full build only)

The Full build runs gensrc, a Python script that auto-generates C++ source
files. Python 3 must be installed and `.py` files must be associated with the
Python executable.

If `.py` files are not associated with Python on your machine, edit
`ObjectHandler\gensrc\Makefile.vc` and `QuantLibAddin\gensrc\Makefile.vc`
and set the `PYTHON` variable to the full path of your Python executable, e.g.:

```
PYTHON=C:\Users\username\AppData\Local\Programs\Python\Python39\python.exe
```

---

## 3 Repository Layout

The solution files use relative paths to locate their dependent projects, so
the repository must be checked out as a single directory tree. The expected
layout (as it appears after cloning this repository with submodules) is:

```
QuantLibAddin\
  gensrc\               # code-generation framework (Full build only)
  log4cxx\              # logging library
  ObjectHandler\        # object repository
  QuantLib\             # QuantLib C++ analytics library (git submodule)
  QuantLibAddin\        # QuantLib C++ wrapper
  QuantLibXL\           # Excel XLL
  boost.props           # Boost location settings - edit this before building
```

---

## 4 Build Steps

### Step 1 - Edit boost.props

Open `boost.props` in the repository root and update the paths as described
in section 2.2 above.

### Step 2 - Open the solution

- **Basic build**: open `QuantLibXL\QuantLibXL_basic.sln` in Visual Studio.
- **Full build**: open `QuantLibXL\QuantLibXL_full.sln` in Visual Studio.

### Step 3 - Select configuration and platform

In the Visual Studio toolbar, select a configuration and platform:

| Goal | Configuration | Platform |
|---|---|---|
| Dynamic-runtime XLL (requires MSVC runtime on target) | **Release** | **x64** |
| Static-runtime XLL (self-contained, no runtime install needed) | **Release (static runtime)** | **x64** |

The `(static runtime)` configurations link everything — including Boost and the
MSVC runtime — statically into the XLL, so users do not need to install the
Visual C++ Redistributable to run the add-in.

> **Prerequisite for static build:** you must have completed the second conan
> install described in section 2.2 and updated the `BoostConanStatic` property
> group in `boost.props` before selecting this configuration.

### Step 4 - Build

Choose **Build > Build Solution** (or press `Ctrl+Shift+B`).

The build will compile all dependent projects in order:
QuantLib, ObjectHandler (xlsdk, ohxllib), QuantLibAddin (QuantLibObjects),
and finally the XLL itself (QuantLibXLStatic).

For the Full build, the gensrc projects (ohgensrc, qlgensrc) run first to
regenerate source files before compilation.

---

## 5 Output

On a successful build the XLL is written to the `QuantLibXL\xll\` directory.
The filename encodes the configuration:

| Configuration | Output filename |
|---|---|
| Release | `QuantLibXL\xll\QuantLibXL-v145-x64-mt-1_21_0.xll` |
| Release (static runtime) | `QuantLibXL\xll\QuantLibXL-v145-x64-mt-s-1_21_0.xll` |
| Debug | `QuantLibXL\xll\QuantLibXL-v145-x64-mt-gd-1_21_0.xll` |
| Debug (static runtime) | `QuantLibXL\xll\QuantLibXL-v145-x64-mt-sgd-1_21_0.xll` |

The `-s-` suffix (static release) and `-sgd-` suffix (static debug) in the
filename follow the Boost library naming convention and make it easy to
distinguish statically-linked builds from dynamically-linked ones.

---

## 6 Loading the Add-in in Excel

1. Open Excel.
2. Go to **File > Options > Add-ins**.
3. At the bottom, set **Manage** to **Excel Add-ins** and click **Go**.
4. Click **Browse** and navigate to the `.xll` file produced above.
5. Click **OK**.

Alternatively, drag and drop the `.xll` file onto an open Excel workbook.
