# H1-Mod 1.4

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

<br/>

## Disclaimer

This software has been made for learning purposes.
