#pragma once
#include <bitset>
#include <utility>
#include <vector>

class SudokuLogic {
public:
  SudokuLogic();
  void GenerateNewGame(int emptyCellsCount);
  bool SetPlayerCell(int r, int c, char val);
  char GetCell(int r, int c) const;
  bool IsCellLocked(int r, int c) const;
  bool IsGameWon() const;

  void ToggleNote(int r, int c, int val);
  void SetNote(int r, int c, int val, bool active) {
    if (active)
      notes[r][c].set(val);
    else
      notes[r][c].reset(val);
  }
  void ToggleAllNotes(int r, int c);
  bool HasNote(int r, int c, int val) const;

  void ResetToInitial();
  void FillAllNotes();

  const std::vector<std::vector<char>> &GetTrueSolution() const {
    return trueSolution;
  }
  void ClearCell(int r, int c) {
    if (!lockedCells[r][c])
      board[r][c] = '.';
  }
  std::vector<std::vector<char>> GetBoardRaw() const { return board; }
  std::vector<std::vector<std::bitset<10>>> GetNotesRaw() const {
    return notes;
  }
  void SetBoardRaw(const std::vector<std::vector<char>> &b) { board = b; }
  void SetNotesRaw(const std::vector<std::vector<std::bitset<10>>> &n) {
    notes = n;
  }

private:
  std::vector<std::vector<char>> board;
  std::vector<std::vector<char>> trueSolution;
  std::vector<std::vector<bool>> lockedCells;
  std::vector<std::vector<std::bitset<10>>> notes;
  std::vector<std::bitset<10>> rows;
  std::vector<std::bitset<10>> cols;
  std::vector<std::bitset<10>> boxes;

  void ResetSolverState();
  void SetBitset(int i, int j, int num, bool value);
  bool IsValid(int i, int j, int num);
  std::pair<int, int> FindEmptyCell();
  bool SolveRandomized();
  int CountSolutionsRecursive(int limit, int &count);
};