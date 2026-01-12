@echo off
setlocal

echo ========================================
echo   NumInter - Release Build Script
echo ========================================
echo.

:: Set up build directory
set BUILD_DIR=build-release
set OUTPUT_DIR=NumInter-Release

:: Clean previous builds
if exist %BUILD_DIR% rmdir /s /q %BUILD_DIR%
if exist %OUTPUT_DIR% rmdir /s /q %OUTPUT_DIR%

:: Create build directory
mkdir %BUILD_DIR%
cd %BUILD_DIR%

:: Configure with CMake (Release mode)
echo [1/4] Configuring CMake...
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
if errorlevel 1 (
    echo ERROR: CMake configuration failed!
    pause
    exit /b 1
)

:: Build
echo.
echo [2/4] Building...
cmake --build . --config Release
if errorlevel 1 (
    echo ERROR: Build failed!
    pause
    exit /b 1
)

:: Create distribution folder
echo.
echo [3/4] Creating distribution package...
cd ..
mkdir %OUTPUT_DIR%
copy %BUILD_DIR%\NumInter.exe %OUTPUT_DIR%\
xcopy /E /I assets %OUTPUT_DIR%\assets

:: Create readme for end users
echo.
echo [4/4] Generating readme...
(
echo NumInter - Number System Converter
echo ===================================
echo.
echo A visual tool to convert between Decimal, Binary, Hexadecimal, and Octal.
echo.
echo HOW TO RUN:
echo   Double-click NumInter.exe
echo.
echo FEATURES:
echo   - Real-time conversion between number systems
echo   - Interactive 8-bit visual builder
echo   - Click on bits to toggle them
echo.
echo REQUIREMENTS:
echo   - Windows 10/11 ^(64-bit^)
echo.
echo Enjoy!
) > %OUTPUT_DIR%\README.txt

echo.
echo ========================================
echo   BUILD SUCCESSFUL!
echo ========================================
echo.
echo Distribution package created in: %OUTPUT_DIR%\
echo.
echo Files included:
dir /b %OUTPUT_DIR%
echo.
pause
