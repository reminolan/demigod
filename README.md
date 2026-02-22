# demigod
A Nintendo Gameboy emulator written in C23 using SDL3, developed by Remi Nolan.

# building
Demigod is built using CMake and uses SDL3 as the only dependency.
The CMake script will automatically detect if the repository was cloned recursively by checking for SDL3's CMakeLists.txt in the vendor directory.
Alternatively, SDL3 can be installed via your platforms package manager (where applicable).

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

> Note for Visual Studio users: MSVC famously doesn't really support the C Language, and is therefore unsupported.

## Why Demigod?
The codename for the Nintendo Gameboy was the DMG, so DeMiGod; but that titlecasing feels very 2005

