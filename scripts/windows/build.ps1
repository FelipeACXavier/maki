param(
    [switch]$Clean,
    [switch]$Release,
    [switch]$NoTest,
    [switch]$Docs,
    [switch]$List,
    [switch]$Help,
    [string]$Prefix = '',
    [string]$Target = '',
    [Parameter(ValueFromRemainingArguments = $true)]
    [string[]]$ExtraArgs
)

if ($Help) {
    Get-Help $PSCommandPath -Detailed
    exit 0
}

# Load the shared settings (including logging and MSVC setup).
$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$SettingsPath = Join-Path $RepoRoot 'scripts\windows\settings.ps1'
. $SettingsPath

if ($Docs) {
    Fail 'Docs are currently not supported on Windows.'
}

$BuildType = if ($Release) { 'Release' } else { 'Debug' }
$BuildPath = Join-Path $BuildPath $BuildType
$BuildTesting = if ($NoTest) { 'OFF' } else { 'ON' }

if ($Prefix) {
    $QtBase = $Prefix
    $QtBin = Join-Path $QtBase 'bin'
    $WindeployqtPath = Join-Path $QtBin 'windeployqt.exe'
    $env:PATH = "$QtBin;$env:PATH"
}

if ($Clean) {
    LogInfo "Cleaning build directory: $BuildPath"
    if (Test-Path -LiteralPath $BuildPath) {
        Remove-Item -LiteralPath $BuildPath -Recurse -Force -ErrorAction Stop
    }
    # Deliberately leave build/.venv-tools intact.
    exit 0
}

LogInfo '==> Windows build script starting...'
LogDebug "Qt version: $QtVersion"
LogDebug "Repo root: $RepoRoot"
LogDebug "Build path: $BuildPath"
LogDebug "Qt prefix: $QtBase"
LogDebug "Install path: $InstallPath"

if (-not (Get-Command cmake.exe -ErrorAction SilentlyContinue)) {
    Fail 'CMake not found. Run setup.ps1 before building.'
}

if ($List) {
    if (-not (Test-Path -LiteralPath $BuildPath)) {
        Fail "Build directory does not exist: $BuildPath. Configure MAKI first."
    }
    & cmake --build $BuildPath --config $BuildType --target help
    exit $LASTEXITCODE
}

$VcpkgToolchain = Join-Path $VcPkgPath 'scripts\buildsystems\vcpkg.cmake'
if (-not (Test-Path -LiteralPath $VcpkgToolchain -PathType Leaf)) {
    Fail "vcpkg toolchain file not found: $VcpkgToolchain"
}
if (-not (Test-Path -LiteralPath $QtBase -PathType Container)) {
    Fail "Qt installation not found at $QtBase. Run setup.ps1 or specify -Prefix."
}

$BuildArgs = @()
if ($Target) {
    $BuildArgs += @('--target', $Target)
} elseif ($Release) {
    $BuildArgs += @('--target', 'deploy-windows')
}

if (-not (Test-Path -LiteralPath $BuildPath)) {
    New-Item -ItemType Directory -Path $BuildPath -Force | Out-Null
}

# Keep the generator explicit and consistent with the msvc2022_64 Qt package.
# If you adopt VS 2026 as the required toolchain, update this setting together
# with the rest of the Windows build configuration.
$CmakeArgs = @(
    '-S', $RepoRoot,
    '-B', $BuildPath,
    '-G', 'Visual Studio 17 2022',
    '-A', 'x64',
    '-DDEPLOY_TARGET=windows',
    "-DCMAKE_PREFIX_PATH=$QtBase",
    "-DCMAKE_INSTALL_PREFIX=$InstallPath",
    "-DBUILD_TESTING=$BuildTesting",
    "-DCMAKE_TOOLCHAIN_FILE=$VcpkgToolchain"
)
if ($ExtraArgs) {
    $CmakeArgs += $ExtraArgs
}

LogDebug '==> Configuring CMake...'
& cmake @CmakeArgs
if ($LASTEXITCODE -ne 0) {
    Fail 'CMake configure failed.'
}

LogDebug "==> Building ($BuildType)..."
& cmake --build $BuildPath --config $BuildType --parallel 4 @BuildArgs
if ($LASTEXITCODE -ne 0) {
    Fail 'CMake build failed.'
}

LogInfo '==> build.ps1 finished successfully.'
LogInfo "Build directory: $BuildPath"
exit 0
