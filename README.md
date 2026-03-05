# demigod
A Nintendo Gameboy emulator written in C23 using SDL3, developed by Remi Nolan.

# building
Demigod is built using CMake and uses SDL3 as the only dependency.

The CMake script will first try to pull SDL3 from 'vendor/SDL' which will be populated if you cloned the repository recursively.
Should that fail, the script will assume SDL3-devel is installed system-wide (i.e. through dnf) and reference it using `find_package`.
Otherwise the script will emit a message and fail to configure.

To build run the following commands from the repository root:

```sh
cmake -B build -S .
cmake --build build
```

I recommend building using Ninja, in which case you would run the following commands:

```sh
cmake -B build -S . -G Ninja
cmake --build build
```

> Note for Visual Studio users: MSVC [doesn't fully support C23](https://en.cppreference.com/w/c/compiler_support/23.html), and is therefore unsupported.

## License
Demigod is licensed under the Zlib license.
See LICENSE for more information.

## Why the name Demigod?
The codename for the original gameboy model was DMG so I picked the name DeMiGod, but that capitalisation felt too tacky.

