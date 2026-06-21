# Japanese Puzzle Collection

An elegant, high-performance logic puzzle suite featuring **Sudoku** and **Nonogram** (Picross). Crafted for puzzle enthusiasts and developers alike, this project highlights robust automated solving algorithms and seamless interactive visualizations.

## Features

- **Classic Puzzle Engines:** Full implementations of both Sudoku and Nonograms with scalable difficulty logic.
- **Algorithmic Auto-Solvers:** Deep logic engines capable of solving grids step-by-step using human-like deductive reasoning (e.g., Hidden Singles, Naked Singles, Line/Block Elimination).
- **Interactive Visualizations:** Watch the solver "think". The UI highlights rows, columns, and target cells in real-time as inferences are made.
- **Smart Hinting System:** Context-aware hints that provide exact reasoning and color-coded multi-target elimination.
- **Adjustable Animation Curves:** Control solver playback speeds ranging from subtle 1x speed to an ultra-fast 16x simulation.
- **Clean Architecture:** Strict separation between core game logic, user interface state, and algorithmic solvers.

## Included Games

### Sudoku
A staple mathematical placement puzzle. Fill the 9×9 grid so that every row, column, and 3×3 subgrid contains all digits from 1 to 9.
* Featuring note-taking tools, instant conflict-checking, and one-click multi-note elimination for streamlined gameplay.

### Nonogram (Picross)
A picture logic puzzle in which cells in a grid must be colored or left blank according to numeric clues at the side of the grid to reveal a hidden picture.
* Highlights exact row and column clue rationale during auto-solve sweeps. 

## Architecture

The game collection is developed using modern cross-platform development paradigms, isolating the logic solvers from the renderer. Removing code duplication natively allows deep-stepping through game logic without bogging down the rendering thread.

### Source Highlights:
* `games/Sudoku/SudokuSolver`: Contains the core back-tracking and logical deduction tree for Sudoku.
* `games/Nonogram/NonogramSolver`: Implements permutation testing and overlap scanning for strict Nonogram deductions.
* `core/BaseGameScreen`: The centralized abstract backbone managing lifecycle and UI animation playback speeds across the suite.

## Scaling Potential

The strict modular architecture allows for easy expansion. Because the core grid, navigation systems, and base UI layers are abstracted out, the collection has huge potential for scaling. Additional logic puzzle games like **Kakuro**, **Hitori**, **Slitherlink**, or **Nurikabe** can be integrated seamlessly by simply:
1. Extending the `BaseGameScreen` core class.
2. Providing a unique constraint solver algorithm.
3. Hooking the new game module into the main UI routing.

## Build Instructions

This project is built using C++17 and CMake, leveraging the Raylib library for rendering. To assemble and run the project locally, follow these steps:

### Prerequisites (Raylib System Dependencies)

Before building, you must install a C++ compiler, CMake, and the necessary system development libraries. **For Linux, Raylib requires X11, Wayland, OpenGL, and Audio development packages to compile correctly.**

**Ubuntu / Debian:**
```bash
sudo apt update
sudo apt install build-essential cmake git
sudo apt install libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev libwayland-dev libxkbcommon-dev
```

**Fedora:**
```bash
sudo dnf install gcc-c++ cmake git
sudo dnf install alsa-lib-devel mesa-libGL-devel libX11-devel libXrandr-devel libXi-devel libXcursor-devel libXinerama-devel wayland-devel libxkbcommon-devel
```

**macOS:**
```bash
xcode-select --install
brew install cmake
```

**Windows:**
Install [Visual Studio](https://visualstudio.microsoft.com/) (with C++ Desktop Development) or [MinGW-w64](https://www.mingw-w64.org/), along with [CMake](https://cmake.org/download/).

### Building

1. **Create a Build Directory and Configure:**
   ```bash
   mkdir build && cd build
   cmake ..
   ```
   *(Note: CMake will automatically fetch and link Raylib during the configure step)*

2. **Compile the Project:**
   ```bash
   cmake --build .
   ```
   *The executable built from the source will be generated in the build directory.*

3. **Run the Program:**
   ```bash
   ./japanese-game-collection
   ```