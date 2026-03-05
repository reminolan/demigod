# demigod
A Nintendo Gameboy emulator written in C23 using SDL3, developed by Remi Nolan.

---

# building
Demigod is built using CMake and uses SDL3 as the only dependency.

### dependencies

The CMake script will first try to pull SDL3 from 'vendor/SDL' which will be populated if you cloned the repository recursively.
Should that fail, the script will assume SDL3-devel is installed system-wide (i.e. through dnf) and reference it using `find_package`.
Otherwise the script will emit a message and fail to configure.

If you want to install all dependencies through your package manager, you would use the following commands.

apt (Debian):

```sh
sudo apt-get update
sudo apt-get install cmake SDL3-devel
```

dnf (RHEL):

```sh
sudo dnf update
sudo dnf install cmake SDL3-devel
```

pacman (Arch):

```sh
sudo pacman -ySu cmake SDL3-devel
```

Given the above paragraph, you don't strictly need `SDL3-devel` if you clone the repository recursively, however that will make sure you have all of SDL3's dependencies installed.

### using CMake

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

---

## License
Demigod is licensed under the Zlib license.
See LICENSE for more information.

---

## FAQ

### Why the name Demigod?
The codename for the original gameboy model was DMG so I picked the name DeMiGod, but that capitalisation felt too tacky.

### Why build a new Gameboy Emulator?
This project is not intended to be a replacement for something like VisualBoyAdvance or whichever mature, feature-tested gameboy (family) emulator.
My intent in developing demigod is for me to expand my knowledge of programming and computer hardware.

### All the instructions are for linux, will you support platform X, Y, or Z?
Officially the only supported platform is linux.
However, considering all memory access is endian-aware & SDL3 is the only direct dependency (even the c-runtime is accessed through SDL) it should compile and run on all platforms SDL supports.
That said, it is entirely up to you to compile the emulator and figure out any bugs, macOS and Windows are outside of the scope of this project.

