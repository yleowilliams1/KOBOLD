# KOBOLD

NOTE: KOBOLD is currently **NOT** for public use!

## What is KOBOLD?

KOBOLD stands for: KOBOLD Operates By Only Lean Data.

KOBOLD is a generic library for Computer-Role-Playing-Games (CRPGs). The core goal is to provide a simple interface for common CRPG utilities like: ECS, data driven components, items, turn queue, dice rolls, A* pathfinding, etc. KOBOLD does no rendering and is purely a data interface. You're expected to use your preferred graphics API to draw to the screen yourself with data provided by KOBOLD.

KOBOLD doesn't include external libraries other than the standard C libraries. Alongside this, KOBOLD enforces compatibility across systems.

You can include and exclude headers as you need. KOBOLD tries to be as modular as possible, but please check which headers include which others in the dependency list below.

## Unity

KOBOLD vendors Unity with it's LICENSE.md untouched. It includes the following unaltered files:

- unity.c
- unity.h
- unity_internals.h

You can find the files hosted at: https://github.com/ThrowTheSwitch/Unity

## What's in KOBOLD?

KOBOLD features the following:

- Data Driven ECS [under development]
- AStar pathfinding [under development]
- Data Driven Item/Inventory system [under development]
- Combat Manager [under development]
- Data based dice resolution [under development]
- Custom Dialogue Tokenizer and Runner [under development]
- Generic Serializer [under development]
- Generic Data Driven RPG System System [under development]
- Generic Data Driven Effects System [under development]
- Generic Event Logger [under development]
- Hex/Square Grid Map [under development]
- Entity AI [under development]
- Generic Quest System [under development]
- Scripting Hooks for all the above [under development]

Note: KOBOLD features unit tests with the Unity library.
## Dependencies

- ECS:
- AStar:
- Item/Inventory:
- Combat: ECS - AStar - Item/Inventory

## Techincal Details

Requirements:
All: CMake 3.20+, and a C11 Compiler
macOS: Xcode Command Line Tools
Linux: gcc or clang, make or ninja
Windows: Visual Studio 2019 16.8+ (MSVC), or MinGW-w64

### macOS & Linux:

#### Compile Main
`
cmake -B build-release -DCMAKE_BUILD_TYPE=Release -DKOBOLD_SANITIZERS=OFF
cmake --build build-release
cd build-release
./app
`
#### Compile Main Debug
`
cmake -B build
cmake --build build
cd build
./app
`
#### Compile Tests
`
cmake -B build-test -DKOBOLD_BUILD_GAME=OFF -DKOBOLD_BUILD_TESTS=ON
cmake --build build-test
cd build-test/tests
./test_runner <suite> # See available suits below
./test_runner --list 
ctest --test-dir build-test --output-on-failure
`
## Windows with Visual Studio (MSVC)

#### Compile Main
`
cmake -B build-release -DKOBOLD_SANITIZERS=OFF
cmake --build build-release --config Release
build\Debug\app.exe
`

#### Compile Main Debug
`
cmake -B build
cmake --build build --config Debug
build\Debug\app.exe
`
#### Compile Tests
`
cmake -B build-test -DKOBOLD_BUILD_GAME=OFF -DKOBOLD_BUILD_TESTS=ON
cmake --build build-test --config Debug
build-test\tests\Debug\test_runner.exe
build-test\tests\Debug\test_runner.exe game
ctest --test-dir build-test -C Debug --output-on-failure
`
## Windows with MinGW-w64 (gcc) 

#### Compile Main
`
cmake -B build -G "MinGW Makefiles"
cmake --build build
build\app.exe
`

#### Compile Main Debug
`
cmake -B build
cmake --build build --config Debug
build\Debug\app.exe
`
#### Compile Tests
`
cmake -B build-test -G "MinGW Makefiles" -DKOBOLD_BUILD_GAME=OFF -DKOBOLD_BUILD_TESTS=ON
cmake --build build-test
build-test\tests\test_runner.exe
ctest --test-dir build-test --output-on-failure
`


## Available Test Suits

- None
