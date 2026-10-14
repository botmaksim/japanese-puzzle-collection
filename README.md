# Japanese Puzzle Collection: Deductive Sudoku & Nonogram Suite

[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B17)
[![CMake 3.16+](https://img.shields.io/badge/CMake-3.16%2B-blue.svg)](https://cmake.org/)
[![Algorithms](https://img.shields.io/badge/Algorithms-Logic%20Solvers-brightgreen.svg)](#)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

An algorithmic logic puzzle suite implementing human-like automated deductive solving routines for Sudoku and Nonograms. Written in C++17, the application features step-by-step inference tracking, visual candidate elimination, and contextual hint generation without relying on brute-force search.

---

## Table of Contents
1. [Supported Puzzles & Algorithms](#supported-puzzles--algorithms)
2. [Architecture](#architecture)
3. [Prerequisites](#prerequisites)
4. [Build & Execution](#build--execution)
5. [Directory Structure](#directory-structure)
6. [License](#license)

---

## Supported Puzzles & Algorithms

### Sudoku Engine
* **Naked & Hidden Singles**: Direct deductive cell identification.
* **Locked Candidates**: Pointing and claiming line-box reductions.
* **Naked & Hidden Subsets**: Pairs, triples, and quads elimination.
* **Inference Tracking**: Explains step-by-step logic behind every move.

### Nonogram Engine
* **Line Overlap Deduction**: Calculates definite cell bounds based on row and column numeric clues.
* **Contradiction Checking**: Validates potential fill operations against orthogonal constraints.

---

## Architecture

* **`core`**: Grid representation, bitmask candidate states, and constraint propagation rules.
* **`games`**: Solver logic for Sudoku and Nonogram puzzle types.
* **`screens`**: Interactive interface for manual play, puzzle generation, and automated solver visualization.

---

## Prerequisites

* Compiler: GCC 9+ or Clang 10+ with C++17 support
* Build System: CMake 3.16+

---

## Build & Execution

```bash
git clone git@github.com:botmaksim/japanese-puzzle-collection.git
cd japanese-puzzle-collection

cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

./build/japanese-puzzle-collection
```

---

## Directory Structure

```text
japanese-puzzle-collection/
├── CMakeLists.txt           # Primary build configuration
├── src/
│   ├── main.cpp             # Entry point
│   ├── core/                # Grid data structures and candidate bitmasks
│   ├── games/               # Sudoku and Nonogram deductive solvers
│   └── screens/             # Interactive terminal and UI screens
```

---

## License

This project is open-source software licensed under the [MIT License](LICENSE).
