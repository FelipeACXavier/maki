@page building_windows Building Windows

> [!IMPORTANT]
> I haven't fully checked the process in Windows. The instructions below indicate how it **should** work, but there are not guarantees.

## Building

Before running any of the commands, you need to make sure you are in a PowerShell that can run scripts. 
This can be achieved with the following command:
```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
```

Once that is done, you can proceed with the build process:

<ol start="1"> <li>Run the setup scripts to install the necessary tools (this only needs to be done once).</li></ol>

```powershell
.\scripts\windows\setup_tools.ps1
```

<ol start="2"> <li>Run the setup scripts to install the necessary packages (this only needs to be done once).</li></ol>

```powershell
.\scripts\windows\setup_tools.ps1
```

<ol start="3"> <li>If you are building KODA as part of maki, you need to change the directory to koda and install the necessary Antlr dependencies</li></ol>

```powershell
cd 3rdparty/plugins/koda
 C:\vcpkg\vcpkg install
```

<ol start="4"> <li>With the tools and packages installed we can build the app</li></ol>

```powershell
.\scripts\windows\build.ps1
```

<ol start="4"> <li>Finally, to install the tool</li></ol>

```powershell
.\scripts\windows\release.ps1
```

## Running

After the installation, the executable can be found in `.\release\windows\maki.exe`
