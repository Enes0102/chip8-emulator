<div align="center">

# CHIP-8 Emulator

**English** | [Türkçe](README.tr.md)

A CHIP-8 emulator written from scratch in C++17 with SDL2.<br>
Implements all 35 original instructions and passes the opcode and flag tests of the [Timendus CHIP-8 test suite](https://github.com/Timendus/chip8-test-suite).

![Br8kout running in the emulator](assets/br8kout.png)

</div>

## About

CHIP-8 is a simple virtual machine designed in the 1970s to make writing games for early microcomputers like the COSMAC VIP easier. It never existed as real hardware: only its specification did. This project rebuilds that machine in software, so programs and games written for it more than 40 years ago can run on a modern PC.

The emulator reads a ROM file into memory and runs it instruction by instruction, using the same **fetch → decode → execute** cycle that real CPUs use.

## Why I Built This

I built this project to learn C++ and to understand how a CPU actually works under the hood. Before this, I had never written C++.

It was developed **with the help of AI**: I used an AI assistant to plan the architecture, write and explain the code step by step, and debug build issues. Along the way I learned how memory, registers, the stack and the fetch → decode → execute cycle work, and how to set up a C++ project with CMake and SDL2.

## Screenshots

| IBM logo | Opcode test (Timendus suite) |
| :---: | :---: |
| ![IBM logo](assets/ibm.png) | ![Opcode test results](assets/corax.png) |

## Controls

The 16-key CHIP-8 keypad is mapped to the left side of the keyboard:

```
CHIP-8 keypad        Keyboard
 1 2 3 C              1 2 3 4
 4 5 6 D              Q W E R
 7 8 9 E              A S D F
 A 0 B F              Z X C V
```

| Key | Action |
| --- | --- |
| `Esc` | Quit |

Each game uses its own subset of the keypad. `W` `A` `S` `D`, `Q` and `E` are the most common.

## Features

- **All 35 CHIP-8 instructions**: jumps, subroutines, arithmetic and logic, drawing, keyboard input, timers and memory operations
- **Tested** against the Timendus test suite (IBM logo, opcode and flag tests)
- **Accurate sprite drawing** with XOR rendering and collision detection
- **Delay and sound timers** running at 60 Hz
- **Built-in font** for the hexadecimal digits 0–F
- **Original COSMAC VIP behaviour** for the instructions that differ between interpreters (see below)
- **Scaled window** with a retro green-on-black colour scheme
- **No manual dependency setup**: CMake downloads and builds SDL2 automatically

### Implementation Notes

Some instructions behave differently between CHIP-8 interpreters. This emulator follows the original COSMAC VIP:

| Instruction | Behaviour |
| --- | --- |
| `8XY6` / `8XYE` | Shift `VY` and store the result in `VX` |
| `FX55` / `FX65` | Increment `I` after storing or loading registers |
| `8XY1` / `8XY2` / `8XY3` | Reset `VF` to 0 |
| `DXYN` | Clip sprites at the screen edges instead of wrapping |
| `FX0A` | Wait for a key to be pressed and released |

The CPU runs at about 700 instructions per second (11 per 60 Hz frame).

## Building

**Requirements**

- A C++17 compiler (MSVC, GCC or Clang)
- CMake 3.16 or newer
- Git (CMake uses it to download SDL2)

**Windows (Visual Studio)**

Install the *Desktop development with C++* workload in Visual Studio, then run the following from a *Developer PowerShell*, or open the folder in VS Code with the **CMake Tools** extension and press `F7`:

```powershell
git clone https://github.com/Enes0102/chip8-emulator.git
cd chip8-emulator
cmake -S . -B build
cmake --build build
```

The executable is created at `build\Debug\chip8.exe`.

**Linux / macOS**

```bash
git clone https://github.com/Enes0102/chip8-emulator.git
cd chip8-emulator
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

## Usage

```bash
chip8 path/to/rom.ch8
```

ROM files are not included in this repository. Some good sources:

- [Timendus/chip8-test-suite](https://github.com/Timendus/chip8-test-suite): test ROMs
- [JohnEarnest/chip8Archive](https://github.com/JohnEarnest/chip8Archive): modern public domain games

## Project Structure

```
chip8-emulator/
├── assets/            # Screenshots used in the README
├── src/
│   ├── chip8.h        # Chip8 class: memory, registers, stack, timers and display
│   ├── chip8.cpp      # ROM loading and the fetch/decode/execute cycle
│   └── main.cpp       # SDL window, rendering, keyboard input and main loop
├── CMakeLists.txt     # Build configuration (fetches SDL2)
├── README.md          # English documentation
└── README.tr.md       # Turkish documentation
```

## Roadmap

- [ ] Sound (beep while the sound timer is active)
- [ ] Configurable speed and quirks from the command line
- [ ] SUPER-CHIP support

## Built With

- C++17
- SDL2
- CMake

## License

This project is licensed under the [MIT License](LICENSE).

## Author

**Enescan Güleryüz** — [@Enes0102](https://github.com/Enes0102)
