@page building_windows Building Windows

> [!IMPORTANT]
> I haven't fully checked the process in Windows. The instructions below indicate how it **should** work, but there are not guarantees.

## Building

<ol start="1"> <li>Run the setup script to install the necessary tools (this only needs to be done once).</li></ol>

```powershell
.\scripts\windows\setup.ps1
```

<ol start="2"> <li>Once that is done, we can build the app</li></ol>

```powershell
.\scripts\windows\build.ps1
```

<ol start="3"> <li>Finally, to install the tool</li></ol>

```powershell
.\scripts\windows\release.ps1
```

## Running

After the installation, the executable can be found in `.\release\windows\maki.exe`
