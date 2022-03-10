# H1-Mod

<p align="center">
  <img alig src="https://cdn.discordapp.com/attachments/895680402142941194/938488726391242842/splash.png"/>
</p>

An experimental client for Call of Duty Modern Warfare Remastered (version 1.04).  
Support for the latest version (1.15) is planned for the future.  
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
- [mjkzy](https://github.com/mjkzy) - porting code from s1x
- [Skull](https://github.com/skkuull) - initial porting of s1x

## Disclaimer

This software has been created purely for the purposes of academic research. It is not intended to be used to attack other systems. Project maintainers are not responsible or liable for misuse of the software. Use responsibly.
