cmake_minimum_required(VERSION 3.11)
project(NumInter C)

set(CMAKE_C_STANDARD 11)

# ============== BUILD TYPE ==============
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# ============== RAYLIB CONFIGURATION ==============
set(RAYLIB_INCLUDE_DIR "C:/raylib/include")
set(RAYLIB_LIB_DIR "C:/raylib/lib")

if(NOT EXISTS ${RAYLIB_INCLUDE_DIR})
    message(FATAL_ERROR
        "\nRAYLIB NOT FOUND! Download from:\n"
        "https://github.com/raysan5/raylib/releases/download/5.0/raylib-5.0_win64_mingw-w64.zip\n"
        "Extract to C:/raylib/\n"
    )
endif()

include_directories(${RAYLIB_INCLUDE_DIR})
link_directories(${RAYLIB_LIB_DIR})

# ============== EXECUTABLE ==============
if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release")
    # WIN32 makes it a GUI application (no console window)
    add_executable(NumInter WIN32 main.c)
else()
    add_executable(NumInter main.c)
endif()

# ============== LINKING ==============
if(WIN32)
    # Static linking for portable Windows release
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_link_options(NumInter PRIVATE -static -static-libgcc)
    endif()
    target_link_libraries(NumInter raylib opengl32 gdi32 winmm)
else()
    target_link_libraries(NumInter raylib m pthread dl)
endif()

# ============== COPY ASSETS ==============
add_custom_command(TARGET NumInter POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:NumInter>/assets
    COMMENT "Copying assets to output directory"
)
