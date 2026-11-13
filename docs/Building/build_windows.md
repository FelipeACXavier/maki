@page building_windows Building Windows

[TOC]

> [!IMPORTANT]
> The Windows build procedure has not yet been fully validated across different machines. Please report any issues you encounter.

## Prerequisites

- Windows with PowerShell.
- **Visual Studio 2022** or **Build Tools for Visual Studio 2022**, with the **Desktop development with C++** workload and an x64 MSVC toolchain. The current build script uses CMake's `Visual Studio 17 2022` generator, and Qt is installed for `msvc2022_64`.
- An internet connection for the initial dependency installation.
- Run the setup from an **elevated PowerShell** (Run as Administrator). A normal PowerShell is sufficient for subsequent builds.

You do **not** need to install Python manually: setup reuses an installed Python **3.12 or newer**, or installs Python 3.12 if no compatible interpreter is found.

## Building

Open PowerShell **at the MAKI repository root**. If PowerShell prevents local scripts from running, enable them for the current session only:

```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
```

### 1. Install the build tools and Qt

Run the setup script from an **elevated PowerShell**:

```powershell
.\scripts\windows\setup.ps1
```

This script automatically invokes `setup_tools.ps1` to:

- Install or locate Chocolatey, Git, CMake, and Ninja.
- Reuse a compatible Python (3.12+) or install Python 3.12 if needed.
- Create or reuse a dedicated Python virtual environment at `build\.venv-tools`.
- Install `aqtinstall` **inside** that virtual environment (not into your system Python).
- Install or bootstrap vcpkg at `C:\vcpkg`.
- Install the Qt version defined in `.qt-version` under `C:\Qt`.

The script also checks whether MSVC is available. If it warns that `cl.exe` is missing, install or repair the Visual Studio C++ workload before building.

> [!NOTE]
> You only need to run setup for the initial installation or when dependencies change. There is no need to activate the Python virtual environment: the scripts call its Python executable directly.
>
> `setup_tools.ps1` can still be invoked separately when you only want to prepare the tooling:
> ```powershell
> .\scripts\windows\setup_tools.ps1
> ```

### 2. Install KODA dependencies (when building KODA)

If you are building the KODA plugin, install its vcpkg dependencies from its directory:

```powershell
Push-Location .\3rdparty\plugins\koda
& C:\vcpkg\vcpkg.exe install
Pop-Location
```

These commands assume the KODA plugin directory contains the vcpkg manifest used by the project. `Pop-Location` returns you to the MAKI repository root.

### 3. Build MAKI

Open a normal PowerShell at the repository root and run:

```powershell
.\scripts\windows\build.ps1
```

The default build uses Debug configuration. Other useful options include:

```powershell
# Build without tests
.\scripts\windows\build.ps1 -NoTest

# Build a specific target
.\scripts\windows\build.ps1 -Target kodagen

# List targets (after a successful CMake configuration)
.\scripts\windows\build.ps1 -List

# Delete the Debug build directory only
.\scripts\windows\build.ps1 -Clean
```

The build artifacts are placed under `build\windows\Debug` by default.

### 4. Build and deploy the Release version

```powershell
.\scripts\windows\build.ps1 -Release
```

By default, the Release command builds the `deploy-windows` target. Its configured installation directory is `release\windows`.

## Running

After a successful Release deployment, MAKI should be available at:

```text
.\release\windows\maki.exe
```

## Python virtual environment

The Python environment is dedicated to **build tooling**, currently `aqtinstall`. It does not replace the system Python installation and does not need to be activated before building MAKI.

To check the environment:

```powershell
.\build\.venv-tools\Scripts\python.exe --version
.\build\.venv-tools\Scripts\python.exe -m pip list
```

If the environment is missing or damaged, recreate it by removing its directory and rerunning setup **from an elevated PowerShell**:

```powershell
Remove-Item -Recurse -Force .\build\.venv-tools
.\scripts\windows\setup.ps1
```

This does not remove the system Python installation. `build.ps1 -Clean` also leaves the tools environment untouched.

> [!TIP]
> If it is not already excluded, add `/build/` (or at least `/build/.venv-tools/`) to `.gitignore`. Virtual environments contain machine-specific paths and should not be committed.
