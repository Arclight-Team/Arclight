<img src="assets/logo.png" height="150" align="right">

# Arclight - High-performance C++20 ecosystem

[![Codacy Badge](https://app.codacy.com/project/badge/Grade/424f1b0554d8435e90b66ee8bcca0c36)](https://www.codacy.com/gh/Arclight-Team/Arclight/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=Arclight-Team/Arclight&amp;utm_campaign=Badge_Grade)
[![GitHub Stars](https://badgen.net/github/stars/Arclight-Team/Arclight)](https://github.com/Arclight-Team/Arclight/stargazers/)
![Version](https://img.shields.io/badge/version-unreleased-yellow)
![Standard](https://img.shields.io/badge/C%2B%2B-20-blueviolet)
![License](https://img.shields.io/badge/license-none-red)

## General
Arclight is an ecosystem intended to help developers in creating new C++ projects quickly without having to setup a common codebase everytime.
It offers a wide range of features, ranging from basic utilities over image manipulation up to 3D rendering.

Additionally, all code is written in C++20 by making heavy use of new concepts introduced in later C++ standards.
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
- Image manipulation library (+ BMP loading)
- Mathematics (Vectors, Matrices, Quaternions, Beziers, Fading and more)
- Logging & Debugging
- Timing utilities
- Stream readers/writers
- Unicode conversion support
- Compile-time encryption/hashing (DES, TripleDES, MD5, SHAx)
- Additional containers/extensions to std (BitSpan, OptionalRef, improved Any)
- XML parser
- Filesystem library
- System classes (FileDialog, MessageBox, Notifications)
- Type traits + Concepts
- Configuration & Platform macros
- Optional runtime for easier setup
- Actor-Component-System (ACS)

Following features are being worked on/are planned:
- Unified code examples
- Spring, our 2D Sprite Engine
- Command-line argument parser
- UnicodeString
- Compression algorithms (+ DEFLATE)
- Encryption algorithms
- Arclight Sound System (AS2)
- Physics engine
- Advanced imaging formats (GIF/PNG/JPEG)
- Full font render engine with TTF/OTF support
- Noise library (extension to imaging)
- OS information query
- Full 3D render engine

## Support
If you want to support this project, feel free to donate as soon as donation is set up.
If you consider joining our team, contact us through arcayn.arclight@gmail.com. 

## Credits
- [Arcayn](https://github.com/Arcaynx) for the project's management
- [Ed_IT](https://github.com/Ed-1T) for the help as the second head developer
- [ItzTacos](https://github.com/ItzTacosOfficial) for the logo, testing the code and several additions / fixes
- [KonPet](https://github.com/KonPet) for the great compression support + PNG management
