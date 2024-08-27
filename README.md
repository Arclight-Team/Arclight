<img src="Assets/Logo.png" height="140" align="left" style="border: 10px solid transparent;" alt="Arclight Logo">
<h1>Arclight<br><sub>High-performance modern C++ ecosystem</sub><br><br></h1>

[![GitHub Stars](https://badgen.net/github/stars/Arclight-Team/Arclight)](https://github.com/Arclight-Team/Arclight/stargazers/)
[![Version](https://badgen.net/static/version/unreleased/yellow)]()
[![Standard](https://badgen.net/static/c++%20standard/23/purple)]()
[![License](https://badgen.net/github/license/Arclight-Team/Arclight)]()

> [!IMPORTANT]
> Arclight is under heavy development right now.
> Due to that, interfaces might drastically change and most of the code is yet to be documented.
> This will change with the first full stable release of the ecosystem.
> Information provided in this README may be outdated; it is going to get updated in the near future.

## General

Arclight is an ecosystem intended to help developers in creating new C++ projects quickly without having to setup a common codebase everytime.
It offers a wide range of features, ranging from basic utilities over image manipulation up to 3D rendering.

Additionally, all code is written in C++23 by making heavy use of new concepts introduced in later C++ standards.
No code cruft, no old libraries from over 20 years, no license confusion: Arclight unifies all aspects of application development into one toolbox.

The code is not aimed towards a particular audience; Instead, everyone can write applications with it, no matter your level of profession or skill.
Everything was written to be as clear as possible without requiring too much prior knowledge on the user's side.

### So, why would you want to use Arclight?

Because development has never been that simple. We aim at maximum flexibility, user experience and performance so that you can get exactly what you want.

## Features

The following features have been fully implemented:
- Full window support (including Cursor + opt. Animations)
- OpenGL 3D rendering backend GLE
- Advanced input management
- Image manipulation library, with full .bmp support, baseline JPEG
- Mathematics (Vectors, Matrices, Quaternions, etc.)
- Fastest C++23 Noise library for Perlin, Simplex and Worley noise up to 4D
- Logging & Debugging
- Command-line argument parser
- Timing utilities
- Byte/Bit readers/writers
- Unicode conversion support
- Compile-time encryption/hashing (DES, TripleDES, MD5, SHAx)
- Additional containers/extensions to std (BitSpan, OptionalRef, improved Any)
- Sound backend ASX
- XML parser
- Filesystem library
- OS classes: FileDialog, MessageBox, Notifications, Process
- Type traits + Concepts
- Configuration & Platform macros
- Optional runtime for easier setup
- Windows manifest skeleton
- Actor-Component-System (ACS)

Following features are being worked on:
- Spring, our 2D Sprite Engine
- Image formats: png, non-baseline JPEG, gif
- UnicodeString
- Compression algorithms
- Encryption algorithms such as AES and Blowfish/Twofish
- Arclight Sound System (AS2)
- Network management
- Full font render engine with TTF/OTF support
- Noise editor for optimized noise execution
- SSE/AVX acceleration of vectorizable functions
- `constexpr` Math

Planned features:
- Physics engine
- OS information query
- Full 3D render engine
- Unified code examples
- Image encoding
- Video streaming

## Dependencies

Due to our handcrafted implementations optimized for performance, there is barely any need for external libraries.
Those that are necessary include:
- GLFW for Window/Input and the render context
- GLEW for OpenGL extension management
- Bullet3 for physics
- FMOD for AS2

Only those libraries used by a module being built are required to build an application.
All other dependencies appearing in `CMakeLists.txt` are temporary and will be removed with the first release of Arclight.

## Building

Arclight is using CMake to build all of its modules. The preliminary buildsystem requires one extra file named `arclight.modules` residing in the Arclight root directory, containing the list of modules to be built. Note that some modules need to be built together.

```
arclight.input
arclight.window
#... add more modules on each line
```

Libraries go into `lib/LibraryName/`. Then, simply invoke CMake to build all of the requested modules.

## Release

The first release is expected to be in 2023. Until then, there will be a consistent, intuitive and well-tested API, examples for different use cases and removal of unused dependencies.
We also plan an easy-to-use build system through Arclight Studio, a tool to manage different aspects of application development with Arclight and C++.
With the first release the license will be revealed as well.

## Support

If you want to support this project, feel free to donate as soon as donation is set up.
If you consider joining our team, contact us through arcayn.arclight@gmail.com.

## Credits

- [Arcayn](https://github.com/Arcaynx) for the project's management
- [Ed_IT](https://github.com/Ed-1T) for the help as the second head developer
- [ItzTacos](https://github.com/ItzTacosOfficial) for the logo, testing the code and several additions / fixes
- [KonPet](https://github.com/KonPet) and [TheGameratorT](https://github.com/TheGameratorT) for their contributions
