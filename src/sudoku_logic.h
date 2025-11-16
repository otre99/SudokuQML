#ifndef SUDOKU_LOGIC_H
#define SUDOKU_LOGIC_H
#include <string>
#include <unordered_map>
#include <vector>
namespace sk {

using namespace std;

class Board {
  int m_board[9][9] = {0};

 public:
  Board();
  int& operator()(const int i, const int j) { return m_board[i][j]; }
  int operator()(const int i, const int j) const { return m_board[i][j]; }
  int operator()(const int index) const {
    return m_board[index / 9][index % 9];
  }
  int operator()(const pair<int, int>& pos) const {
    return m_board[pos.first / 9][pos.second];
  }
  int emptyCellsCount() const;

  void fromFile(const string& fname);
  void toFile(const string& fname) const;
};

class CandidatesBoard {
  bool m_board[9][9][10] = {false};

 public:
  enum Ele { Cell3x3, Col, Row };
  static CandidatesBoard fromBoard(const Board& board);
  bool* operator()(const int i, const int j) { return m_board[i][j]; }
  const bool* operator()(const int i, const int j) const {
    return m_board[i][j];
  }

  bool* operator()(const int index) { return m_board[index / 9][index % 9]; }
  bool* operator()(const pair<int, int>& pos) {
    return m_board[pos.first][pos.second];
  }

  vector<int> freq(int index, Ele element) const;
  vector<int> freq(int cellIndex, int index, Ele element) const;
  unordered_map<int, vector<int> > nakedPairs(int index, Ele element) const;

  void partialUpdateFromCellValue(int row, int col, int value);
  void partialUpdateFromCellValue(int index, int value) {
    partialUpdateFromCellValue(index / 9, index % 9, value);
  }

  void updateCandidate(int row, int col, int c, bool add);
  void updateCandidate(int index, int c, bool add) {
    updateCandidate(index / 9, index % 9, c, add);
  }
  void setCandidates(int row, int col, const std::vector<int>& c);
  void setCandidates(int index, const std::vector<int>& c) {
    setCandidates(index / 9, index % 9, c);
  }

  std::vector<int> candidates(int row, int col) const;
  std::vector<int> candidates(int index) const {
    return candidates(index / 9, index % 9);
  }
  CandidatesBoard();

  // utils
  static vector<int> indices(Ele element, int elementIndex);
  static vector<int> subIndicesIn3x3Cell(int ele3x3Index, Ele element,
                                         int subIndex);
};

std::vector<int> getCandidates(int row, int col, const Board& board);
bool isValid(int row, int col, int value, const Board& board);
bool isValidSolution(Board& board);
int countSolutions(Board& board, int limit = 2);
bool generateSolution(Board& board, int cellIndex);
bool completeSolution(Board& board, int cellIndex);
Board generateSolvableBoard(int maxFilledCells, int level, bool& success,
                            Board* solvedBoard = nullptr);

// find dificulty

int findBoardDificultyLevel(Board& board);
bool solveWithMediumStrategy(Board& board);
bool solveWithEasyStrategy(Board& board);

bool tryEasyStrategy1(Board& board, CandidatesBoard& cBoard);
bool tryEasyStrategy2(Board& board, CandidatesBoard& cBoard);
bool tryMediumStrategy1(Board& board, CandidatesBoard& cBoard);
bool tryMediumStrategy2(Board& board, CandidatesBoard& cBoard);
}  // namespace sk
#endif  // SUDOKU_LOGIC_H
