<#
.SYNOPSIS
    Build MAKI on Windows.

.DESCRIPTION
    Configures and builds MAKI using CMake and MSVC.
    Supports Debug and Release builds, and cleaning build directories.

.PARAMETER Clean
    Remove the selected build directory without building.

.PARAMETER Release
    Build MAKI in Release mode instead of Debug.

.PARAMETER NoTest
    Disable building tests.

.PARAMETER Prefix
    Override the Qt installation prefix.

.PARAMETER Docs
    Build documentation.

.PARAMETER List
    List available CMake targets.

.PARAMETER Target
    Build a specific CMake target.

.EXAMPLE
    .\build.ps1 -Release -Target kodagen

.EXAMPLE
    .\build.ps1 -NoTest -Docs

.EXAMPLE
    .\build.ps1 -List

.EXAMPLE
    .\build.ps1
    Build MAKI in Debug mode.

.EXAMPLE
    .\build.ps1 -Release
    Build MAKI in Release mode.

.EXAMPLE
    .\build.ps1 -Clean
    Remove the Debug build directory.

.EXAMPLE
    .\build.ps1 -Clean -Release
    Remove the Release build directory.

.NOTES
    Requires MSVC, CMake, Qt, and vcpkg.
#>

param(
  [switch]$Clean,
  [switch]$Release,
  [switch]$NoTest,
  [switch]$Docs,
  [switch]$List,
  [switch]$Help,

  [string]$Prefix = "",
  [string]$Target = "",

  [Parameter(ValueFromRemainingArguments = $true)]
  [string[]]$ExtraArgs
)

if ($Help) {
  Get-Help $PSCommandPath -Detailed
  exit 0
}

if ($Docs) {
  Fail "Docs are currently not supported on Windows"
}

# ------------------------------------------------------
# Load shared settings
# ------------------------------------------------------
$RepoRoot = Resolve-Path (Join-Path $PSScriptRoot "..\..")
$SettingsPath = Join-Path $RepoRoot "scripts\windows\settings.ps1"
. $SettingsPath

$BuildType = if ($Release) { "Release" } else { "Debug" }
$BuildPath = Join-Path $BuildPath "$BuildType"

if ($Prefix) {
  $QtBase = $Prefix
}

$BuildTesting = if ($NoTest) { "OFF" } else { "ON" }

if ($Clean) {
  Write-Host "Cleaning build directory: $BuildPath"
  if (Test-Path $BuildPath) {
    Remove-Item -Recurse -Force $BuildPath
  }
  exit 0
}

LogInfo "==> Windows build script starting..."

# ------------------------------------------------------
# Configuration
# ------------------------------------------------------
LogInfo "Using Qt version: $QtVersion"
LogDebug "Repo root: $RepoRoot"

# ------------------------------------------------------
# List targets
# ------------------------------------------------------
if ($List) {
  cmake --build "$BuildPath" --config $BuildType --target help
  exit $LASTEXITCODE
}

$BuildArgs = ""

if ($Target) {
  $BuildArgs += @("--target", $Target)
} elseif ($Release) {
  $BuildArgs += @("--target", "deploy-windows")
}

LogDebug "RepoRoot: $RepoRoot"
LogDebug "BuildPath: $BuildPath"
LogDebug "QtBase: $QtBase"
LogDebug "InstallPath: $InstallPath"
LogDebug "ExtraArgs: $ExtraArgs"
LogDebug "BuildArgs: $BuildArgs"

# ------------------------------------------------------
# Configure & build with CMake
# ------------------------------------------------------
if (-not (Test-Path $RepoRoot)) {
    Fail "Repo root '$RepoRoot' does not exist."
}

LogDebug "==> Configuring CMake..."
if (-not (Test-Path $BuildPath)) {
    New-Item -ItemType Directory -Path $BuildPath | Out-Null
}

cmake -S "$RepoRoot" -B "$BuildPath" `
  -A x64 `
  -DDEPLOY_TARGET="windows" `
  -DCMAKE_PREFIX_PATH="$QtBase" `
  -DCMAKE_INSTALL_PREFIX="$InstallPath" `
  -DCMAKE_BUILD_TYPE="$BuildType" `
  -DBUILD_TESTING="$BuildTesting" `
  -DCMAKE_TOOLCHAIN_FILE="$VcPkgPath\scripts\buildsystems\vcpkg.cmake" `
  $ExtraArgs

if ($LASTEXITCODE -ne 0) {
  Fail "CMake configure failed."
}

LogDebug "==> Building ($BuildType)..."
cmake --build "$BuildPath" `
      --config "$BuildType" `
      --parallel 4 `
      $BuildArgs

if ($LASTEXITCODE -ne 0) {
  Fail "CMake configure failed."
}

LogInfo "==> build_windows.ps1 finished successfully." -ForegroundColor Green
LogInfo "Build directory: $BuildPath"
exit 0
