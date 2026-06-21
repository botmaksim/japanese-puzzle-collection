#include "NonogramLogic.h"
#include <random>
#include <raylib.h>

bool SolveLineDP(const std::vector<int> &clues,
                 const std::vector<CellState> &line,
                 std::vector<CellState> &resultLine) {
  int N = line.size();
  int K = clues.size();
  resultLine = line;

  if (K == 0) {
    bool possible = true;
    for (int i = 0; i < N; ++i)
      if (line[i] == CellState::FILLED)
        possible = false;
    if (possible) {
      for (int i = 0; i < N; ++i)
        resultLine[i] = CellState::CROSSED;
      return true;
    }
    return false;
  }

  auto run_dp = [&](const std::vector<int> &cls,
                    const std::vector<CellState> &ln) {
    int n = ln.size();
    int k = cls.size();
    std::vector<std::vector<bool>> dp(k + 1, std::vector<bool>(n + 1, false));
    dp[0][0] = true;
    for (int i = 1; i <= n; ++i) {
      dp[0][i] = dp[0][i - 1] && (ln[i - 1] != CellState::FILLED);
    }
    for (int c = 1; c <= k; ++c) {
      int L = cls[c - 1];
      for (int i = 1; i <= n; ++i) {
        if (ln[i - 1] != CellState::FILLED && dp[c][i - 1]) {
          dp[c][i] = true;
        }
        if (i >= L) {
          bool canPlace = true;
          for (int j = i - L; j < i; ++j) {
            if (ln[j] == CellState::CROSSED) {
              canPlace = false;
              break;
            }
          }
          if (canPlace) {
            if (c == 1) {
              if (dp[0][i - L])
                dp[c][i] = true;
            } else {
              if (i - L >= 1 && ln[i - L - 1] != CellState::FILLED &&
                  dp[c - 1][i - L - 1]) {
                dp[c][i] = true;
              }
            }
          }
        }
      }
    }
    return dp;
  };

  auto dp = run_dp(clues, line);
  if (!dp[K][N])
    return false;

  std::vector<int> rev_clues(clues.rbegin(), clues.rend());
  std::vector<CellState> rev_line(line.rbegin(), line.rend());
  auto rev_dp = run_dp(rev_clues, rev_line);

  std::vector<bool> canCross(N, false);
  for (int i = 0; i < N; ++i) {
    if (line[i] == CellState::FILLED)
      continue;
    for (int c = 0; c <= K; ++c) {
      if (dp[c][i] && rev_dp[K - c][N - 1 - i]) {
        canCross[i] = true;
        break;
      }
    }
  }

  std::vector<bool> canFill(N, false);
  for (int c = 0; c < K; ++c) {
    int L = clues[c];
    for (int start = 0; start <= N - L; ++start) {
      bool canPlace = true;
      for (int j = start; j < start + L; ++j) {
        if (line[j] == CellState::CROSSED) {
          canPlace = false;
          break;
        }
      }
      if (!canPlace)
        continue;

      bool prefixOk = false;
      if (c == 0) {
        prefixOk = dp[0][start];
      } else {
        if (start >= 1 && line[start - 1] != CellState::FILLED &&
            dp[c][start - 1]) {
          prefixOk = true;
        }
      }
      if (!prefixOk)
        continue;

      bool suffixOk = false;
      int remaining_clues = K - 1 - c;
      int suffix_len = N - (start + L);
      if (remaining_clues == 0) {
        suffixOk = rev_dp[0][suffix_len];
      } else {
        if (suffix_len >= 1 && line[start + L] != CellState::FILLED &&
            rev_dp[remaining_clues][suffix_len - 1]) {
          suffixOk = true;
        }
      }
      if (!suffixOk)
        continue;

      for (int j = start; j < start + L; ++j) {
        canFill[j] = true;
      }
    }
  }

  for (int i = 0; i < N; ++i) {
    if (line[i] == CellState::EMPTY) {
      if (canFill[i] && !canCross[i])
        resultLine[i] = CellState::FILLED;
      else if (!canFill[i] && canCross[i])
        resultLine[i] = CellState::CROSSED;
    }
  }
  return true;
}

NonogramLogic::NonogramLogic() : gridSize(5) {}

void NonogramLogic::GenerateNewGame(int size) {
  gridSize = size;

  int maxAttempts = 200;
  if (size >= 10)
    maxAttempts = 50;
  if (size >= 15)
    maxAttempts = 10;
  int bestScore = -1;
  bool foundSolvable = false;
  std::vector<std::vector<bool>> bestSolution;

  for (int attempts = 0; attempts < maxAttempts; ++attempts) {
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
    int iterations = 0;
    while (changed) {
      changed = false;
      iterations++;
      for (int r = 0; r < size; ++r) {
        std::vector<CellState> currentLine = testGrid[r];
        std::vector<CellState> resultLine;
        if (SolveLineDP(GetRowClue(r), currentLine, resultLine)) {
          for (int c = 0; c < size; ++c) {
            if (testGrid[r][c] != resultLine[c]) {
              testGrid[r][c] = resultLine[c];
              changed = true;
            }
          }
        }
      }
      for (int c = 0; c < size; ++c) {
        std::vector<CellState> currentLine(size);
        for (int r = 0; r < size; ++r)
          currentLine[r] = testGrid[r][c];
        std::vector<CellState> resultLine;
        if (SolveLineDP(GetColClue(c), currentLine, resultLine)) {
          for (int r = 0; r < size; ++r) {
            if (testGrid[r][c] != resultLine[r]) {
              testGrid[r][c] = resultLine[r];
              changed = true;
            }
          }
        }
      }
    }

    int filledCount = 0;
    int score = iterations * 1000;
    bool fullySolved = true;
    for (int r = 0; r < size; ++r) {
      for (int c = 0; c < size; ++c) {
        if (testGrid[r][c] == CellState::EMPTY)
          fullySolved = false;
        else
          filledCount++;
      }
    }

    if (!fullySolved) {
      score = filledCount; // if not fully solved, just prioritize how much can
                           // be solved logically
    } else {
      for (int r = 0; r < size; ++r) {
        for (int cl : GetRowClue(r))
          if (cl >= size - 1)
            score -= 500;
      }
      for (int c = 0; c < size; ++c) {
        for (int cl : GetColClue(c))
          if (cl >= size - 1)
            score -= 500;
      }
    }

    if (fullySolved && !foundSolvable) {
      foundSolvable = true;
      bestScore = score;
      bestSolution = solution;
    } else if (fullySolved == foundSolvable) {
      if (score > bestScore) {
        bestScore = score;
        bestSolution = solution;
      }
    }
  }

  if (!bestSolution.empty()) {
    solution = bestSolution;
  }
  CalculateClues();
  playerGrid.assign(size, std::vector<CellState>(size, CellState::EMPTY));
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