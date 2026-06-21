#pragma once
#include <vector>

enum class CellState { EMPTY, FILLED, CROSSED };

bool SolveLineDP(const std::vector<int> &clues,
                 const std::vector<CellState> &line,
                 std::vector<CellState> &resultLine);

class NonogramLogic {
public:
  NonogramLogic();
  void GenerateNewGame(int size);

  void ToggleCell(int r, int c);
  void ToggleCrossCell(int r, int c);
  void ClearCell(int r, int c) { playerGrid[r][c] = CellState::EMPTY; }

  CellState GetCellState(int r, int c) const;
  const std::vector<int> &GetRowClue(int r) const;
  const std::vector<int> &GetColClue(int c) const;

  int GetSize() const;
  bool IsGameWon() const;
  void ResetToInitial();
  int CountSolutions(int limit) const;

  std::vector<std::vector<CellState>> GetPlayerGrid() const {
    return playerGrid;
  }
  void SetPlayerGrid(const std::vector<std::vector<CellState>> &grid) {
    playerGrid = grid;
  }
  std::vector<std::vector<bool>> GetSolutionRaw() const { return solution; }

private:
  int gridSize;
  std::vector<std::vector<bool>> solution;
  std::vector<std::vector<CellState>> playerGrid;

  std::vector<std::vector<int>> rowClues;
  std::vector<std::vector<int>> colClues;

  void CalculateClues();
};