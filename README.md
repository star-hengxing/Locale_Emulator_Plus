# Locale Emulator Plus

- Support 64-bit.
- For visual novel.

# Usage

Register to the right-click context menu. (unsupported windows 11)
```console
$ locale_emulator_plus_tool install
```

# Build

## Prerequisites

- [xmake](https://xmake.io/#/guide/installation)
- Requires C++20 compiler.

## Setup

- [Visual Studio](https://visualstudio.microsoft.com) (If you just want to build without developing, download the [Microsoft C++ Build Tools](https://visualstudio.microsoft.com/visual-cpp-build-tools))

- Recommend use [scoop](https://scoop.sh) as package manager on Windows.

```sh
scoop install xmake
```

## Build

```sh
xmake -y
```

## Test

```sh
xmake test
```

# Credits

- [Locale-Emulator-Core](https://github.com/xupefei/Locale-Emulator-Core)
- [Locale_Remulator](https://github.com/InWILL/Locale_Remulator)
- [hook_finale](https://github.com/rumia-san/hook_finale)
- [Textractor](https://github.com/Artikash/Textractor)
