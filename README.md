# H1-Mod 1.4

- *This project supports only singleplayer for now*
- ***YOU CAN'T** get in multiplayer, networking development is slow.*

<p align="center">
  <img alig src="https://cdn.discordapp.com/attachments/895680402142941194/938488726391242842/splash.png"/>
</p>

Proof of concept for an Modern Warfare Remastered client. <br>
[This project is based on S1x](https://github.com/XLabsProject/s1x-client)

## Compile from source

- Clone the Git repo. Do NOT download it as ZIP, that won't work.
- Update the submodules and run `premake5 vs2019` or simply use the delivered `generate.bat`.
- Build via solution file in `build\h1-mod.sln`.

### Premake arguments

| Argument                    | Description                                    |
|:----------------------------|:-----------------------------------------------|
| `--copy-to=PATH`            | Optional, copy the EXE to a custom folder after build, define the path here if wanted. |
| `--dev-build`               | Enable development builds of the client. |

## Credits:

- [XLabsProject - S1x](https://github.com/XLabsProject/s1x-client), Project base.
- [quaK](https://github.com/Joelrau), Helping in alot of stuff.
- [fed - h2-mod](https://github.com/fedddddd/h2-mod), [Dvars](https://github.com/skkuull/h1-mod/blob/main/src/client/game/dvars.cpp) and [patches](https://github.com/skkuull/h1-mod/blob/main/src/client/component/patches.cpp).


## Disclaimer

This software has been made for learning purposes.
