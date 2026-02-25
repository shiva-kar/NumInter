# NumInter – Number System Converter & Analyzer

A modern, feature-rich desktop application for number base conversion, bitwise operations, and IEEE 754 floating-point analysis — built in pure C with raylib.

![Platform](https://img.shields.io/badge/Platform-Windows-blue) ![License](https://img.shields.io/badge/License-MIT-green) ![raylib](https://img.shields.io/badge/Made%20with-raylib-red) ![Version](https://img.shields.io/badge/Version-2.0.0-orange)

---

## ✨ Features

### 🔢 Number Converter
- **Real-time conversion** between Decimal, Binary, Hexadecimal, and Octal
- **8-bit / 16-bit visual builder** — click individual bits to toggle values
- **Copy to clipboard** on any field with one click
- **ASCII character preview** for values 0–127
- **Conversion history** — click any past entry to reload it

### ⚙️ Bitwise Operation Calculator
- Supports **AND, OR, XOR, NOT, Shift Left, Shift Right**
- Enter operands in decimal, see results in all bases instantly
- One-click copy of results

### 📐 IEEE 754 Floating-Point Analyzer
- Enter any decimal number and see its **32-bit IEEE 754** layout
- Color-coded bit visualization: **sign** (red), **exponent** (blue), **mantissa** (green)
- Detects special values: ±Infinity, NaN, denormalized, ±zero

### 🎨 Modern UI
- **Dark / Light theme toggle** (Ctrl+D)
- Smooth animated bit toggles with easing
- Card-based layout with shadows and rounded corners
- Status bar with live mode info
- Toast notifications for actions
- Keyboard shortcuts for power users

---

## ⌨️ Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| `Tab` | Switch between tabs |
| `Ctrl+1/2/3` | Jump to specific tab |
| `Ctrl+D` | Toggle dark/light theme |
| `Ctrl+L` | Clear current input |

---

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
git clone https://github.com/shiva-kar/NumInter.git
cd NumInter

# Option 1: Use the build script
build_release.bat

# Option 2: Manual CMake build
mkdir build && cd build
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

## 🎮 Input Reference

| Field | Base | Valid Characters | Range |
|-------|------|------------------|-------|
| Decimal | 10 | 0–9 | 0–255 (8-bit) / 0–65535 (16-bit) |
| Binary | 2 | 0, 1 | Up to 16 digits |
| Hexadecimal | 16 | 0–9, A–F | Up to 4 digits |
| Octal | 8 | 0–7 | Up to 6 digits |

## 📁 Project Structure

```
NumInter/
├── main.c                  # Full application (~900 LOC)
├── CMakeLists.txt          # CMake build configuration
├── build_release.bat       # One-click Windows release build
├── LICENSE                 # MIT License
├── assets/
│   └── JetBrainsMonoNerdFont-Bold.ttf
└── README.md
```

## 🛠️ Dependencies

- [raylib](https://www.raylib.com/) 5.0 — Graphics & input library
- [JetBrains Mono Nerd Font](https://www.jetbrains.com/lp/mono/) — Bundled

## 📄 License

MIT License — see [LICENSE](LICENSE) for details.

---

Made with ❤️ using C and raylib
