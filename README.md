<img src="assets/logo.png" height="150" align="right">

# Arclight - High-performance C++23 ecosystem

[![Codacy Badge](https://app.codacy.com/project/badge/Grade/424f1b0554d8435e90b66ee8bcca0c36)](https://www.codacy.com/gh/Arclight-Team/Arclight/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=Arclight-Team/Arclight&amp;utm_campaign=Badge_Grade)
[![GitHub Stars](https://badgen.net/github/stars/Arclight-Team/Arclight)](https://github.com/Arclight-Team/Arclight/stargazers/)
![Version](https://img.shields.io/badge/version-unreleased-yellow)
![Standard](https://img.shields.io/badge/C%2B%2B-20-blueviolet)
![License](https://img.shields.io/badge/license-none-red)

## General
Arclight is an ecosystem intended to help developers in creating new C++ projects quickly without having to setup a common codebase everytime.
It offers a wide range of features, ranging from basic utilities over image manipulation up to 3D rendering.

Additionally, all code is written in C++23 by making heavy use of new concepts introduced in later C++ standards.
No code cruft, no old libraries from over 20 years, no license confusion: Arclight unifies all aspects of application development into one toolbox.

The code is not aimed towards a particular audience; Instead, everyone can write applications with it, no matter your level of profession or skill.
Everything was written to be as clear as possible without requiring too much prior knowledge on the user's side.

### So, why would you want to use Arclight?

Because development has never been that simple. We aim at maximum flexibility, user experience and performance so that you can get exactly what you want.

*Note that Arclight is under heavy development right now.* Interfaces might drastically change on active branches.
Due to that, most of the code is yet to be documented in the near future. If you need stability and consistency, we recommend waiting until the first full release.

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
