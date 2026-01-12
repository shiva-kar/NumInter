# NumInter - Number System Converter

A sleek, visual desktop application for converting numbers between **Decimal**, **Binary**, **Hexadecimal**, and **Octal** systems.

![Number System Converter](https://img.shields.io/badge/Platform-Windows-blue) ![License](https://img.shields.io/badge/License-MIT-green) ![raylib](https://img.shields.io/badge/Made%20with-raylib-red)

## ✨ Features

- **Real-time Conversion** – Type in any field and see instant conversions
- **8-Bit Visual Builder** – Click bits to toggle them and see the value change
- **Clean Modern UI** – Smooth, responsive interface with color-coded fields
- **Portable** – Single executable, no installation required

## 📦 Download

Get the latest release from the [Releases](../../releases) page.

## 🚀 Quick Start

### For Users
1. Download `NumInter-Release.zip` from Releases
2. Extract the archive
3. Run `NumInter.exe`

### For Developers

**Prerequisites:**
- MinGW-w64 (GCC for Windows)
- CMake 3.11+
- raylib 5.0 installed at `C:/raylib/`

**Build from source:**
```batch
# Clone the repository
git clone <repo-url>
cd NumInter

# Option 1: Use the build script
build_release.bat

# Option 2: Manual CMake build
mkdir build && cd build
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

## 🎮 Usage

| Field | Base | Valid Characters |
|-------|------|------------------|
| Decimal | 10 | 0-9 |
| Binary | 2 | 0, 1 |
| Hexadecimal | 16 | 0-9, A-F |
| Octal | 8 | 0-7 |

**Range:** 0 to 255 (8-bit unsigned)

Click on any input field to edit it. Use the visual bit toggles at the bottom to build numbers interactively.

## 📁 Project Structure

```
NumInter/
├── main.c              # Application source code
├── CMakeLists.txt      # CMake build configuration
├── build_release.bat   # Windows release build script
├── assets/
│   └── JetBrainsMonoNerdFont-Bold.ttf
└── README.md
```

## 🛠️ Dependencies

- [raylib](https://www.raylib.com/) 5.0 – Simple and easy-to-use game development library
- [JetBrains Mono](https://www.jetbrains.com/lp/mono/) – Font (bundled)

## 📄 License

This project is open source. Feel free to use, modify, and distribute.

---

Made with ❤️ using raylib
