# App closer

Virus that close any app

Specify target app process in `src/hate.c` in `TARGET_NAME_PROCESS`

You can change start icon via changing icon path in `src/resource.rc` (IDI_APP_ICON) and name in `src/hate.c` (WINDOW_NAME and EXE_NAME).

After run exe it will add himself to autostart to current user + copy into %TEMP% folder.

# Building

You will need 
 - Visual Studio 2017 or newer
 - make tool in your Path

In Visual Studio environment (Developer PowerShell for VS 2022 for example)

```bash
> make # to build. Exe will be in out folder.
> make clear # to clear out folder
```
