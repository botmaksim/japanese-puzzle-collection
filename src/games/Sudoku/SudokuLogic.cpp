#include "SudokuLogic.h"
#include <algorithm>
#include <random>

SudokuLogic::SudokuLogic() {
  board = std::vector<std::vector<char>>(9, std::vector<char>(9, '.'));
  lockedCells = std::vector<std::vector<bool>>(9, std::vector<bool>(9, false));
}

void SudokuLogic::ResetSolverState() {
  rows = std::vector<std::bitset<10>>(9, std::bitset<10>());
  cols = std::vector<std::bitset<10>>(9, std::bitset<10>());
  boxes = std::vector<std::bitset<10>>(9, std::bitset<10>());
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      if (board[i][j] != '.') {
        SetBitset(i, j, board[i][j] - '0', true);
      }
    }
  }
}

void SudokuLogic::SetBitset(int i, int j, int num, bool value) {
  rows[i][num] = value;
  cols[j][num] = value;
  boxes[(i / 3) * 3 + j / 3][num] = value;
}

bool SudokuLogic::IsValid(int i, int j, int num) {
  return !rows[i][num] && !cols[j][num] && !boxes[(i / 3) * 3 + j / 3][num];
}

std::pair<int, int> SudokuLogic::FindEmptyCell() {
  int minOptions = 10;
  std::pair<int, int> result = {-1, -1};
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      if (board[i][j] == '.') {
        int options = 0;
        for (int num = 1; num <= 9; num++) {
          if (IsValid(i, j, num))
            options++;
        }
        if (options < minOptions) {
          minOptions = options;
          result = {i, j};
          if (minOptions == 1)
            return result;
        }
      }
    }
  }
  return result;
}
bool SudokuLogic::SolveRandomized() {
  auto [i, j] = FindEmptyCell();
  if (i == -1)
    return true;

  std::vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle(nums.begin(), nums.end(), g);

  for (int num : nums) {
    if (IsValid(i, j, num)) {
      SetBitset(i, j, num, true);
      board[i][j] = '0' + num;
      if (SolveRandomized())
        return true;
      SetBitset(i, j, num, false);
      board[i][j] = '.';
    }
  }
  return false;
}

void SudokuLogic::GenerateNewGame(int emptyCellsCount) {
  int bestRemoved = -1;
  std::vector<std::vector<char>> bestBoard;
  std::vector<std::vector<char>> bestTrueSolution;

  for (int attempt = 0; attempt < 200; ++attempt) {
    board = std::vector<std::vector<char>>(9, std::vector<char>(9, '.'));
    lockedCells =
        std::vector<std::vector<bool>>(9, std::vector<bool>(9, false));
    ResetSolverState();
    SolveRandomized();

    trueSolution = board;
    std::random_device rd;
    std::mt19937 g(rd());
    int removed = 0;
    std::vector<std::pair<int, int>> allCells;
    for (int r = 0; r < 9; ++r) {
      for (int c = 0; c < 9; ++c) {
        allCells.push_back({r, c});
      }
    }
    std::shuffle(allCells.begin(), allCells.end(), g);

    for (auto &cell : allCells) {
      if (removed >= emptyCellsCount)
        break;
      int r = cell.first;
      int c = cell.second;
      char backup = board[r][c];
      board[r][c] = '.';

      ResetSolverState();
      int count = 0;
      CountSolutionsRecursive(2, count);
      if (count > 1) {
        board[r][c] = backup;
      } else {
        removed++;
      }
    }

    if (removed > bestRemoved) {
      bestRemoved = removed;
      bestBoard = board;
      bestTrueSolution = trueSolution;
    }
    if (removed >= emptyCellsCount) {
      break;
    }
  }

  board = bestBoard;
  trueSolution = bestTrueSolution;
  for (int i = 0; i < 9; ++i) {
    for (int j = 0; j < 9; ++j) {
      lockedCells[i][j] = (board[i][j] != '.');
    }
  }

  notes = std::vector<std::vector<std::bitset<10>>>(
      9, std::vector<std::bitset<10>>(9, std::bitset<10>()));
}

int SudokuLogic::CountSolutionsRecursive(int limit, int &count) {
  auto [i, j] = FindEmptyCell();
  if (i == -1) {
    count++;
    return count;
  }

  for (int num = 1; num <= 9; ++num) {
    if (IsValid(i, j, num)) {
      SetBitset(i, j, num, true);
      board[i][j] = '0' + num;
      if (CountSolutionsRecursive(limit, count) >= limit) {
        SetBitset(i, j, num, false);
        board[i][j] = '.';
        return count;
      }
      SetBitset(i, j, num, false);
      board[i][j] = '.';
    }
  }
  return count;
}

bool SudokuLogic::SetPlayerCell(int r, int c, char val) {
  if (lockedCells[r][c])
    return false;
  board[r][c] = val;
  return true;
}

char SudokuLogic::GetCell(int r, int c) const { return board[r][c]; }
bool SudokuLogic::IsCellLocked(int r, int c) const { return lockedCells[r][c]; }

void SudokuLogic::ToggleNote(int r, int c, int val) {
  if (lockedCells[r][c])
    return;
  if (val >= 1 && val <= 9) {
    notes[r][c].flip(val);
  }
}

void SudokuLogic::ToggleAllNotes(int r, int c) {
  if (lockedCells[r][c])
    return;
  bool empty = true;
  for (int i = 1; i <= 9; i++) {
    if (notes[r][c][i]) {
      empty = false;
      break;
    }
  }

  if (empty) {
    for (int i = 1; i <= 9; i++)
      notes[r][c].set(i);
  } else {
    notes[r][c].reset();
  }
}

bool SudokuLogic::HasNote(int r, int c, int val) const {
  if (r < 0 || r >= 9 || c < 0 || c >= 9)
    return false;
  if (val < 1 || val > 9)
    return false;
  return notes[r][c][val];
}

void SudokuLogic::ResetToInitial() {
  for (int r = 0; r < 9; ++r) {
    for (int c = 0; c < 9; ++c) {
      if (!lockedCells[r][c]) {
        board[r][c] = '.';
        notes[r][c].reset();
      }
    }
  }
}

void SudokuLogic::FillAllNotes() {
  for (int r = 0; r < 9; ++r) {
    for (int c = 0; c < 9; ++c) {
      if (board[r][c] == '.') {
        for (int i = 1; i <= 9; ++i)
          notes[r][c].set(i);
      }
    }
  }
}

bool SudokuLogic::IsGameWon() const {
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      if (board[i][j] == '.')
        return false;
    }
  }
  for (int i = 0; i < 9; i++) {
    std::bitset<10> r_set, c_set, b_set;
    for (int j = 0; j < 9; j++) {
      int r_val = board[i][j] - '0';
      int c_val = board[j][i] - '0';
      int b_val = board[(i / 3) * 3 + j / 3][(i % 3) * 3 + j % 3] - '0';

      if (r_val < 1 || r_val > 9 || r_set[r_val])
        return false;
      r_set[r_val] = true;

      if (c_val < 1 || c_val > 9 || c_set[c_val])
        return false;
      c_set[c_val] = true;

      if (b_val < 1 || b_val > 9 || b_set[b_val])
        return false;
      b_set[b_val] = true;
    }
  }

  return true;
}