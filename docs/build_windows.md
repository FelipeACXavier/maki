# Windows setup

> [!IMPORTANT]
> I haven't fully checked the process in Windows. The instructions below indicate how it **should** work, but there are not guarantees.

## Building

1. Run the setup script to install the necessary tools (this only needs to be done once).

```powershell
.\scripts\windows\seyup.ps1
```

2. Once that is done, we can build the app

```powershell
.\scripts\windows\build.ps1
```

3. Finally, to install the tool

```powershell
.\scripts\windows\release.ps1
```

## Running

After the installation, the executable can be found in `.\release\windows\maki.exe`
