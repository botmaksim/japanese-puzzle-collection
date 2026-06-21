#include "NonogramLogic.h"
#include <random>
#include <raylib.h>

void GetPermsInline(size_t clueIdx, size_t pos, const std::vector<int> &clues,
                    const std::vector<CellState> &currentLine,
                    std::vector<CellState> &currentPerm,
                    std::vector<std::vector<CellState>> &validPerms) {
  if (clueIdx == clues.size()) {
    for (size_t i = pos; i < currentLine.size(); ++i) {
      if (currentLine[i] == CellState::FILLED)
        return;
      currentPerm[i] = CellState::CROSSED;
    }
    validPerms.push_back(currentPerm);
    return;
  }

  int remainingBlocks = clues.size() - clueIdx - 1;
  int minRequired = 0;
  for (size_t i = clueIdx; i < clues.size(); ++i)
    minRequired += clues[i];
  minRequired += remainingBlocks;

  int maxStart = currentLine.size() - minRequired;
  for (int start = pos; start <= maxStart; ++start) {
    bool canPlace = true;
    for (int i = pos; i < start; ++i) {
      if (currentLine[i] == CellState::FILLED) {
        canPlace = false;
        break;
      }
    }
    if (!canPlace)
      break;

    for (int i = 0; i < clues[clueIdx]; ++i) {
      if (currentLine[start + i] == CellState::CROSSED) {
        canPlace = false;
        break;
      }
    }
    if (start + clues[clueIdx] < currentLine.size()) {
      if (currentLine[start + clues[clueIdx]] == CellState::FILLED)
        canPlace = false;
    }

    if (canPlace) {
      for (int i = pos; i < start; ++i)
        currentPerm[i] = CellState::CROSSED;
      for (int i = 0; i < clues[clueIdx]; ++i)
        currentPerm[start + i] = CellState::FILLED;
      if (start + clues[clueIdx] < currentLine.size())
        currentPerm[start + clues[clueIdx]] = CellState::CROSSED;

      GetPermsInline(clueIdx + 1, start + clues[clueIdx] + 1, clues,
                     currentLine, currentPerm, validPerms);
    }
  }
}

NonogramLogic::NonogramLogic() : gridSize(5) {}

void NonogramLogic::GenerateNewGame(int size) {
  gridSize = size;
  bool isLogicallySolvable = false;

  int maxAttempts = 200;
  if (size >= 10)
    maxAttempts = 200;
  if (size >= 15)
    maxAttempts = 200;
  int attempts = 0;

  while (!isLogicallySolvable && attempts < maxAttempts) {
    attempts++;
    solution.assign(size, std::vector<bool>(size, false));
    int threshold = 50;
    if (size >= 10)
      threshold = 52;
    if (size >= 15)
      threshold = 55;

    for (int r = 0; r < size; ++r) {
      for (int c = 0; c < size; ++c) {
        solution[r][c] = (GetRandomValue(0, 100) > threshold);
      }
    }
    for (int i = 0; i < size; ++i) {
      bool rowEmpty = true, colEmpty = true;
      for (int j = 0; j < size; ++j) {
        if (solution[i][j])
          rowEmpty = false;
        if (solution[j][i])
          colEmpty = false;
      }
      if (rowEmpty)
        solution[i][size / 2] = true;
      if (colEmpty)
        solution[size / 2][i] = true;
    }

    CalculateClues();
    playerGrid.assign(size, std::vector<CellState>(size, CellState::EMPTY));

    // Fast logic solver check
    auto testGrid = playerGrid;
    bool changed = true;
    bool fullySolved = true;
    while (changed) {
      changed = false;
      for (int r = 0; r < size; ++r) {
        std::vector<CellState> currentLine = testGrid[r];
        std::vector<CellState> currentPerm(size, CellState::EMPTY);
        std::vector<std::vector<CellState>> validPerms;
        GetPermsInline(0, 0, GetRowClue(r), currentLine, currentPerm,
                       validPerms);
        if (validPerms.empty())
          continue;
        for (int c = 0; c < size; ++c) {
          if (testGrid[r][c] == CellState::EMPTY) {
            bool allFilled = true, allCrossed = true;
            for (const auto &perm : validPerms) {
              if (perm[c] != CellState::FILLED)
                allFilled = false;
              if (perm[c] != CellState::CROSSED)
                allCrossed = false;
            }
            if (allFilled) {
              testGrid[r][c] = CellState::FILLED;
              changed = true;
            } else if (allCrossed) {
              testGrid[r][c] = CellState::CROSSED;
              changed = true;
            }
          }
        }
      }
      for (int c = 0; c < size; ++c) {
        std::vector<CellState> currentLine(size);
        for (int r = 0; r < size; ++r)
          currentLine[r] = testGrid[r][c];
        std::vector<CellState> currentPerm(size, CellState::EMPTY);
        std::vector<std::vector<CellState>> validPerms;
        GetPermsInline(0, 0, GetColClue(c), currentLine, currentPerm,
                       validPerms);
        if (validPerms.empty())
          continue;
        for (int r = 0; r < size; ++r) {
          if (testGrid[r][c] == CellState::EMPTY) {
            bool allFilled = true, allCrossed = true;
            for (const auto &perm : validPerms) {
              if (perm[r] != CellState::FILLED)
                allFilled = false;
              if (perm[r] != CellState::CROSSED)
                allCrossed = false;
            }
            if (allFilled) {
              testGrid[r][c] = CellState::FILLED;
              changed = true;
            } else if (allCrossed) {
              testGrid[r][c] = CellState::CROSSED;
              changed = true;
            }
          }
        }
      }
    }
    for (int r = 0; r < size; ++r) {
      for (int c = 0; c < size; ++c) {
        if (testGrid[r][c] == CellState::EMPTY)
          fullySolved = false;
      }
    }
    isLogicallySolvable = fullySolved;
  }
}

void NonogramLogic::CalculateClues() {
  rowClues.assign(gridSize, std::vector<int>());
  colClues.assign(gridSize, std::vector<int>());
  for (int r = 0; r < gridSize; ++r) {
    int count = 0;
    for (int c = 0; c < gridSize; ++c) {
      if (solution[r][c])
        count++;
      else if (count > 0) {
        rowClues[r].push_back(count);
        count = 0;
      }
    }
    if (count > 0)
      rowClues[r].push_back(count);
    if (rowClues[r].empty())
      rowClues[r].push_back(0);
  }
  for (int c = 0; c < gridSize; ++c) {
    int count = 0;
    for (int r = 0; r < gridSize; ++r) {
      if (solution[r][c])
        count++;
      else if (count > 0) {
        colClues[c].push_back(count);
        count = 0;
      }
    }
    if (count > 0)
      colClues[c].push_back(count);
    if (colClues[c].empty())
      colClues[c].push_back(0);
  }
}

void NonogramLogic::ToggleCell(int r, int c) {
  if (playerGrid[r][c] == CellState::FILLED)
    playerGrid[r][c] = CellState::EMPTY;
  else
    playerGrid[r][c] = CellState::FILLED;
}

void NonogramLogic::ToggleCrossCell(int r, int c) {
  if (playerGrid[r][c] == CellState::CROSSED)
    playerGrid[r][c] = CellState::EMPTY;
  else
    playerGrid[r][c] = CellState::CROSSED;
}

CellState NonogramLogic::GetCellState(int r, int c) const {
  return playerGrid[r][c];
}
const std::vector<int> &NonogramLogic::GetRowClue(int r) const {
  return rowClues[r];
}
const std::vector<int> &NonogramLogic::GetColClue(int c) const {
  return colClues[c];
}
int NonogramLogic::GetSize() const { return gridSize; }

void NonogramLogic::ResetToInitial() {
  for (int r = 0; r < gridSize; ++r) {
    for (int c = 0; c < gridSize; ++c) {
      playerGrid[r][c] = CellState::EMPTY;
    }
  }
}

bool NonogramLogic::IsGameWon() const {
  for (int r = 0; r < gridSize; ++r) {
    std::vector<int> playerRowClues;
    int count = 0;
    for (int c = 0; c < gridSize; ++c) {
      if (playerGrid[r][c] == CellState::FILLED)
        count++;
      else if (count > 0) {
        playerRowClues.push_back(count);
        count = 0;
      }
    }
    if (count > 0)
      playerRowClues.push_back(count);
    if (playerRowClues.empty())
      playerRowClues.push_back(0);

    if (playerRowClues != rowClues[r])
      return false;
  }
  for (int c = 0; c < gridSize; ++c) {
    std::vector<int> playerColClues;
    int count = 0;
    for (int r = 0; r < gridSize; ++r) {
      if (playerGrid[r][c] == CellState::FILLED)
        count++;
      else if (count > 0) {
        playerColClues.push_back(count);
        count = 0;
      }
    }
    if (count > 0)
      playerColClues.push_back(count);
    if (playerColClues.empty())
      playerColClues.push_back(0);

    if (playerColClues != colClues[c])
      return false;
  }

  return true;
}