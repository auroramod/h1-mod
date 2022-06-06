![license](https://img.shields.io/github/license/h1-mod/h1-mod.svg)
[![open bugs](https://img.shields.io/github/issues/h1-mod/h1-mod/bug?label=bugs)](https://github.com/h1-mod/h1-mod/issues?q=is%3Aissue+is%3Aopen+label%3Abug)
[![Build](https://github.com/h1-mod/h1-mod/workflows/Build/badge.svg)](https://github.com/h1-mod/h1-mod/actions)
[![Build status](https://ci.appveyor.com/api/projects/status/0sh80kdnsvm53rno?svg=true)](https://ci.appveyor.com/project/h1-mod/h1-mod)
[![Discord](https://img.shields.io/discord/945420505157083208?color=%237289DA&label=members&logo=discord&logoColor=%23FFFFFF)](https://discord.gg/RzzXu5EVnh)

# H1-Mod

<p align="center">
  <img src="https://cdn.discordapp.com/attachments/866389597948739594/983104537990692924/splash.png" width="800px"/>
</p>

A client for Call of Duty: Modern Warfare Remastered.

[This project is based on S1x.](https://github.com/XLabsProject/s1x-client)

## Compile from source

- Clone the Git repo. Do NOT download it as ZIP, that won't work.
- Update the submodules and run `premake5 vs2022` or simply use the delivered `generate.bat`.
- Build via solution file in `build\h1-mod.sln`.

### Premake arguments

| Argument                    | Description                                    |
|:----------------------------|:-----------------------------------------------|
| `--copy-to=PATH`            | Optional, copy the EXE to a custom folder after build, define the path here if wanted. |
| `--dev-build`               | Enable development builds of the client. |

## Credits:

- [XLabsProject](https://github.com/XLabsProject) - codebase and iw6x/s1x research
- [quaK](https://github.com/Joelrau) - lots of insight and help
- [fed](https://github.com/fedddddd) - fixed DW/networking, work from [h2-mod](https://github.com/fedddddd/h2-mod)
- [Skull](https://github.com/skkuull) + [mjkzy](https://github.com/mjkzy) - porting code from s1x
- [momo5502](https://github.com/momo5502) - Arxan/Steam research, developer of XLabsProject :D

## Disclaimer

This software has been created purely for the purposes of academic research. It is not intended to be used to attack other systems. Project maintainers are not responsible or liable for misuse of the software. Use responsibly.
