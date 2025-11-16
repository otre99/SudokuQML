#include "sudoku_logic.h"

#include <QDebug>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
using namespace std;

namespace {
void __countSolutions(int& counter, sk::Board& board, const int limit) {
  for (int i = 0; i < 81; ++i) {
    const int row = i / 9;
    const int col = i % 9;
    if (board(row, col)) continue;

    for (int value = 1; value < 10; ++value) {
      if (sk::isValid(row, col, value, board)) {
        board(row, col) = value;
        __countSolutions(counter, board, limit);
        board(row, col) = 0;
        if (counter >= limit) return;
      }
    }
    return;
  }
  ++counter;
}

void __applyOr(vector<bool>& a, const bool b[10]) {
  for (int i = 1; i < 10; ++i) {
    a[i] = a[i] || b[i];
  }
}

void __applyAdd(vector<int>& a, const bool b[10]) {
  for (int i = 1; i < 10; ++i) {
    a[i] = a[i] + b[i];
  }
}

void __applySubtract(vector<int>& a, const bool b[10]) {
  for (int i = 1; i < 10; ++i) {
    a[i] = a[i] - b[i];
  }
}

void __applySubtract(vector<int>& a, const vector<int>& b) {
  for (int i = 1; i < 10; ++i) {
    a[i] = a[i] - b[i];
  }
}

vector<int> __toVec(const bool a[10]) {
  vector<int> r;
  for (int i = 1; i < 10; ++i) {
    if (a[i]) r.push_back(i);
  }
  return r;
}

}  // namespace

namespace sk {

Board::Board() {}

int Board::emptyCellsCount() const {
  int count = 0;
  for (int index = 0; index < 81; ++index) {
    count += m_board[index / 9][index % 9] == 0;
  }
  return count;
}

void Board::fromFile(const string& fname) {
  ifstream ifile(fname);
  for (int i = 0; i < 81; ++i) {
    ifile >> m_board[i / 9][i % 9];
  }
}

void Board::toFile(const string& fname) const {
  ofstream ofile(fname);
  for (int i = 0; i < 81; ++i) {
    ofile << m_board[i / 9][i % 9] << ((i + 1) % 9 == 0 ? '\n' : ' ');
  }
}

CandidatesBoard::CandidatesBoard() {}

CandidatesBoard CandidatesBoard::fromBoard(const Board& board) {
  CandidatesBoard cBoard;
  for (int index = 0; index < 81; ++index) {
    const int row = index / 9;
    const int col = index % 9;
    if (board(row, col) == 0) {
      cBoard.setCandidates(row, col, getCandidates(row, col, board));
    }
  }
  return cBoard;
}

vector<int> CandidatesBoard::freq(int index, Ele element) const {
  vector<int> result(10, 0);
  switch (element) {
    case Cell3x3: {
      const int i0 = (index / 3) * 3;
      const int j0 = (index % 3) * 3;
      for (int i = i0; i < i0 + 3; ++i) {
        for (int j = j0; j < j0 + 3; ++j) {
          ::__applyAdd(result, m_board[i][j]);
        }
      }
      break;
    }
    case Row: {
      for (int k = 0; k < 9; ++k) {
        ::__applyAdd(result, m_board[index][k]);
      }
      break;
    }
    case Col: {
      for (int k = 0; k < 9; ++k) {
        ::__applyAdd(result, m_board[k][index]);
      }
      break;
    }
    default:
      break;
  }
  return result;
}

vector<int> CandidatesBoard::freq(int cellIndex, int index, Ele element) const {
  vector<int> result(10, 0);
  const int i0 = (cellIndex / 3) * 3;
  const int j0 = (cellIndex % 3) * 3;
  switch (element) {
    case Row: {
      for (int k = j0; k < j0 + 3; ++k) {
        ::__applyAdd(result, m_board[i0 + index][k]);
      }
      break;
    }
    case Col: {
      for (int k = i0; k < i0 + 3; ++k) {
        ::__applyAdd(result, m_board[k][j0 + index]);
      }
      break;
    }
    default:
      break;
  }
  return result;
}

unordered_map<int, vector<int>> CandidatesBoard::nakedPairs(int index,
                                                            Ele element) const {
  unordered_map<int, vector<int>> result;
  switch (element) {
    case Cell3x3: {
      const auto indices = CandidatesBoard::indices(Cell3x3, index);
      for (int idx : indices) {
        const auto cds = candidates(idx);
        if (cds.size() == 2) {
          result[cds[0] * 10 + cds[1]].push_back(idx);
        }
      }
      break;
    }
    case Row: {
      for (int k = 0; k < 9; ++k) {
        const auto cds = candidates(index, k);
        if (cds.size() == 2) {
          result[cds[0] * 10 + cds[1]].emplace_back(index * 9 + k);
        }
      }
      break;
    }
    case Col: {
      for (int k = 0; k < 9; ++k) {
        const auto cds = candidates(k, index);
        if (cds.size() == 2) {
          result[cds[0] * 10 + cds[1]].emplace_back(k * 9 + index);
        }
      }
      break;
    }
    default:
      break;
  }
  for (auto it = result.begin(); it != result.end();) {
    if (it->second.size() != 2) {
      it = result.erase(it);
    } else {
      ++it;
    }
  }
  return result;
}

void CandidatesBoard::partialUpdateFromCellValue(int row, int col, int value) {
  const int i0 = (row / 3) * 3;
  const int j0 = (col / 3) * 3;
  int k = 0;
  for (int i = i0; i < i0 + 3; ++i) {
    for (int j = j0; j < j0 + 3; ++j) {
      m_board[i][j][value] = false;
      m_board[row][k][value] = false;
      m_board[k][col][value] = false;
      ++k;
    }
  }
  fill(m_board[row][col], m_board[row][col] + 10, false);
}

void CandidatesBoard::updateCandidate(int row, int col, int c, bool add) {
  m_board[row][col][c] = add;
}

void CandidatesBoard::setCandidates(int row, int col, const vector<int>& c) {
  bool* cs = m_board[row][col];
  fill(cs, cs + 10, false);
  for (auto n : c) {
    cs[n] = true;
  }
}

vector<int> CandidatesBoard::candidates(int row, int col) const {
  return ::__toVec(m_board[row][col]);
}

vector<int> CandidatesBoard::indices(Ele element, int elementIndex) {
  static vector<int> indicesPerCells{};
  if (indicesPerCells.empty()) {
    indicesPerCells.reserve(81);

    for (int index = 0; index < 9; ++index) {
      const int i0 = (index / 3) * 3;
      const int j0 = (index % 3) * 3;
      for (int i = i0; i < i0 + 3; ++i) {
        for (int j = j0; j < j0 + 3; ++j) {
          indicesPerCells.push_back(i * 9 + j);
        }
      }
    }
  }

  switch (element) {
    case Cell3x3: {
      auto it = indicesPerCells.cbegin() + 9 * elementIndex;
      return vector<int>(it, it + 9);
    }
    case Row: {
      const int n = elementIndex * 9;
      return {n + 0, n + 1, n + 2, n + 3, n + 4, n + 5, n + 6, n + 7, n + 8};
    }

    case Col: {
      const int n = elementIndex;
      return {0 + n,  9 + n,  18 + n, 27 + n, 36 + n,
              45 + n, 54 + n, 63 + n, 72 + n};
    }
    default:
      return {};
  }
}

vector<int> CandidatesBoard::subIndicesIn3x3Cell(int ele3x3Index, Ele ele,
                                                 int subIndex) {
  const auto ii = CandidatesBoard::indices(Cell3x3, ele3x3Index);
  switch (ele) {
    case Row: {
      auto it = ii.cbegin() + subIndex * 3;
      return vector<int>(it, it + 3);
    }
    case Col: {
      return {ii[subIndex + 0], ii[subIndex + 1], ii[subIndex + 2]};
    }
    default:
      return {};
  }
}

vector<int> getCandidates(int row, int col, const Board& board) {
  if (board(row, col)) {
    return {};
  }

  bool p9[10];
  fill(p9, p9 + 10, true);
  p9[0] = true;
  const int i0 = (row / 3) * 3;
  const int j0 = (col / 3) * 3;
  // 3x3 cells, row, col
  int k = 0;
  for (int i = i0; i < i0 + 3; ++i) {
    for (int j = j0; j < j0 + 3; ++j) {
      p9[board(i, j)] = false;

      if (k < j0 || k >= j0 + 3) p9[board(row, k)] = false;
      if (k < i0 || k >= i0 + 3) p9[board(k, col)] = false;
      ++k;
    }
  }
  return ::__toVec(p9);
}

int countSolutions(Board& board, int limit) {
  int counter = 0;
  auto cpy = board;
  ::__countSolutions(counter, board, limit);
  return counter;
}

bool isValid(int row, int col, int value, const Board& board) {
  const int i0 = (row / 3) * 3;
  const int j0 = (col / 3) * 3;
  int k = 0;
  for (int i = i0; i < i0 + 3; ++i) {
    for (int j = j0; j < j0 + 3; ++j) {
      if (board(i, j) == value || board(row, k) == value ||
          board(k, col) == value)
        return false;
      ++k;
    }
  }
  return true;
}

bool isValidSolution(Board& board) {
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      const int backup = board(i, j);
      if (backup == 0) return false;
      board(i, j) = 0;
      if (isValid(i, j, backup, board) == false) {
        board(i, j) = backup;
        return false;
      }
      board(i, j) = backup;
    }
  }
  return true;
}

bool generateSolution(Board& board, int cellIndex) {
  if (cellIndex == 81) return true;
  int nums[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  shuffle(nums, nums + 9, mt19937(random_device()()));

  const int r = cellIndex / 9;
  const int c = cellIndex % 9;
  for (int value : nums) {
    if (isValid(r, c, value, board)) {
      board(r, c) = value;
      if (generateSolution(board, cellIndex + 1)) return true;
      board(r, c) = 0;
    }
  }
  return false;
}

bool completeSolution(Board& board, int cellIndex) {
  if (cellIndex == 81) return true;
  int nums[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  shuffle(nums, nums + 9, mt19937(random_device()()));

  while (cellIndex < 81 && board(cellIndex) != 0) ++cellIndex;
  if (cellIndex == 81) return true;

  const int r = cellIndex / 9;
  const int c = cellIndex % 9;
  for (int value : nums) {
    if (isValid(r, c, value, board)) {
      board(r, c) = value;
      if (completeSolution(board, cellIndex + 1)) return true;
      board(r, c) = 0;
    }
  }
  return false;
}

Board generateSolvableBoard(int maxFilledCells, int level, bool& success,
                            Board* solvedBoard) {
  Board board;
  generateSolution(board, 0);

  if (solvedBoard) {
    *solvedBoard = board;
  }

  int filledCells = 81;
  vector<int> indices(81);
  iota(indices.begin(), indices.end(), 0);
  shuffle(indices.begin(), indices.end(), mt19937(random_device()()));

  while (indices.empty() == false) {
    const int index = indices[indices.size() - 1];
    indices.pop_back();

    const int row = index / 9;
    const int col = index % 9;

    const int backup = board(row, col);
    board(row, col) = 0;

    const int nsols = countSolutions(board, 2);

    if (nsols == 1) {
      filledCells -= 1;
      Board tmp = board;

      switch (level) {
        case 0: {
          if (solveWithMediumStrategy(tmp) == false) {
            board(row, col) = backup;
            success = true;
            return board;
          }
          break;
        }
        case 1: {
          if (solveWithMediumStrategy(tmp) == false) {
            success = true;
            // qDebug() << "SAVING!!!!!!!!!!! M\n";
            // board.toFile("/home/rccr/board.txt");
            // tmp.toFile("/home/rccr/medium_board.txt");
            return board;
          }
          break;
        }
        case 2: {
          if (solveWithMediumStrategy(tmp) == false) {
            success = true;
            // qDebug() << "SAVING!!!!!!!!!!! M\n";
            // board.toFile("/home/rccr/board.txt");
            // tmp.toFile("/home/rccr/medium_hard.txt");
            return board;
          }
          break;
        }
        default:
          break;
      }

      if (filledCells <= maxFilledCells) {
        success = false;
        // qDebug() << "NNN!!!!!!!!!!!-----------\n";
        return board;
      }

    } else {
      board(row, col) = backup;
    }
  }
  success = false;
  return board;
}

// scanning
bool tryEasyStrategy1(Board& board, CandidatesBoard& cBoard) {
  for (int index = 0; index < 81; ++index) {
    if (board(index) == 0) {
      auto&& cellCandidates = cBoard.candidates(index);
      if (cellCandidates.size() == 1) {
        board(index / 9, index % 9) = cellCandidates[0];
        cBoard.partialUpdateFromCellValue(index, cellCandidates[0]);
        return true;
      }
    }
  }
  return false;
}

// hidden singles
bool tryEasyStrategy2(Board& board, CandidatesBoard& cBoard) {
  for (int index = 0; index < 9; ++index) {
    // 3x3 cell index
    vector<int> freq = cBoard.freq(index, CandidatesBoard::Cell3x3);
    const vector<int> indices =
        CandidatesBoard::indices(CandidatesBoard::Cell3x3, index);
    for (int idx : indices) {
      if (board(idx)) continue;
      const bool* cds = cBoard(idx);
      ::__applySubtract(freq, cds);

      for (int value = 1; value < 10; ++value) {
        if (cds[value] && freq[value] == 0) {
          board(idx / 9, idx % 9) = value;
          cBoard.partialUpdateFromCellValue(idx, value);
          return true;
        }
      }
      ::__applyAdd(freq, cds);
    }

    vector<int> freq_row = cBoard.freq(index, CandidatesBoard::Row);
    vector<int> freq_col = cBoard.freq(index, CandidatesBoard::Col);
    for (int k = 0; k < 9; ++k) {
      // row
      if (board(index, k) == 0) {
        const bool* cds_row = cBoard(index, k);
        ::__applySubtract(freq_row, cds_row);

        for (int value = 1; value < 10; ++value) {
          if (cds_row[value] && freq_row[value] == 0) {
            board(index, k) = value;
            cBoard.partialUpdateFromCellValue(index, k, value);
            return true;
          }
        }
        ::__applyAdd(freq_row, cds_row);
      }
      // col
      if (board(k, index) == 0) {
        const bool* cds_col = cBoard(k, index);
        ::__applySubtract(freq_col, cds_col);

        for (int value = 1; value < 10; ++value) {
          if (cds_col[value] && freq_col[value] == 0) {
            board(k, index) = value;
            cBoard.partialUpdateFromCellValue(k, index, value);
            return true;
          }
        }
        ::__applyAdd(freq_col, cds_col);
      }
    }
  }
  return false;
}

// naked pairs
bool tryMediumStrategy1(Board& board, CandidatesBoard& cBoard) {
  bool changed = false;
  auto remove_naked = [&](const vector<int>& coords,
                          const unordered_map<int, vector<int>>& nkp) {
    for (auto it = nkp.cbegin(); it != nkp.cend(); ++it) {
      const int c1 = it->first / 10;
      const int c2 = it->first % 10;
      const int pos1 = it->second[0];
      const int pos2 = it->second[1];

      for (auto&& x : coords) {
        if (x != pos1 && x != pos2) {
          bool* cds = cBoard(x);
          if (cds[c1]) {
            changed = true;
            cds[c1] = false;
          }
          if (cds[c2]) {
            changed = true;
            cds[c2] = false;
          }
        }
      }
    }
  };

  for (int index = 0; index < 9; ++index) {
    // cell
    auto nkp = cBoard.nakedPairs(index, CandidatesBoard::Cell3x3);
    if (!nkp.empty()) {
      const auto indices =
          CandidatesBoard::indices(CandidatesBoard::Cell3x3, index);
      remove_naked(indices, nkp);
      if (changed) return true;
    }
    // row
    nkp = cBoard.nakedPairs(index, CandidatesBoard::Row);
    if (!nkp.empty()) {
      const auto indices =
          CandidatesBoard::indices(CandidatesBoard::Row, index);
      remove_naked(indices, nkp);
      if (changed) return true;
    }

    // col
    nkp = cBoard.nakedPairs(index, CandidatesBoard::Col);
    if (!nkp.empty()) {
      const auto indices =
          CandidatesBoard::indices(CandidatesBoard::Col, index);
      remove_naked(indices, nkp);
      if (changed) return true;
    }
  }
  return false;
}

bool tryMediumStrategy2(Board& board, CandidatesBoard& cBoard) {
  bool changed = false;
  // per 3x3 cells
  for (int index = 0; index < 9; ++index) {
    const int i0 = (index / 3) * 3;
    const int j0 = (index % 3) * 3;

    const vector<int> cell3x3freq =
        cBoard.freq(index, CandidatesBoard::Cell3x3);

    for (int k = 0; k < 3; ++k) {
      // row
      const vector<int> rfreq = cBoard.freq(index, k, CandidatesBoard::Row);
      for (int cand = 1; cand < 10; ++cand) {
        if (rfreq[cand] && cell3x3freq[cand] == rfreq[cand]) {
          for (int j = 0; j < 9; ++j) {
            if (j < j0 || j >= j0 + 3) {
              changed |= cBoard(i0 + k, j)[cand];
              cBoard(i0 + k, j)[cand] = false;
            }
          }
        }
      }
      if (changed) return true;

      // all row
      const vector<int> allrow_freq = cBoard.freq(i0 + k, CandidatesBoard::Row);
      for (int cand = 1; cand < 10; ++cand) {
        if (rfreq[cand] && allrow_freq[cand] == rfreq[cand]) {
          for (int i = i0; i < i0 + 3; ++i) {
            if (i == i0 + k) continue;
            for (int j = j0; j < j0 + 3; ++j) {
              changed |= cBoard(i, j)[cand];
              cBoard(i, j)[cand] = false;
            }
          }
        }
      }
      if (changed) return true;

      // col
      const vector<int> cfreq = cBoard.freq(index, k, CandidatesBoard::Col);
      for (int cand = 1; cand < 10; ++cand) {
        if (cfreq[cand] && cell3x3freq[cand] == cfreq[cand]) {
          for (int i = 0; i < 9; ++i) {
            if (i < i0 || i >= i0 + 3) {
              changed |= cBoard(i, j0 + k)[cand];
              cBoard(i, j0 + k)[cand] = false;
            }
          }
        }
      }
      if (changed) return true;

      // all col
      const vector<int> allcol_freq = cBoard.freq(j0 + k, CandidatesBoard::Col);
      for (int cand = 1; cand < 10; ++cand) {
        if (cfreq[cand] && allcol_freq[cand] == cfreq[cand]) {
          for (int j = j0; j < j0 + 3; ++j) {
            if (j == j0 + k) continue;
            for (int i = i0; i < i0 + 3; ++i) {
              changed |= cBoard(i, j)[cand];
              cBoard(i, j)[cand] = false;
            }
          }
        }
      }
      if (changed) return true;
    }

    // per row
  }
  return changed;
}

int findBoardDificultyLevel(Board& board) {
  int lv1Count = 0;
  int lv2Count = 0;
  int lv3Count = 0;
  CandidatesBoard c_board = CandidatesBoard::fromBoard(board);
  while (true) {
    if (tryEasyStrategy1(board, c_board)) {
      lv1Count += 1;
      continue;
    }

    if (tryEasyStrategy2(board, c_board)) {
      lv1Count += 1;
      continue;
    }

    if (tryMediumStrategy1(board, c_board)) {
      lv2Count += 1;
      continue;
    }

    if (tryMediumStrategy2(board, c_board)) {
      lv2Count += 1;
      continue;
    }
    break;
  }
  lv3Count += board.emptyCellsCount();
  qDebug() << lv1Count << " " << lv2Count << " " << lv3Count << '\n';

  // board.toFile(
  //     "/home/rccr/RBT/REPOS/OT/SudokuQML/build/Desktop_Qt_6_9_0-Debug/"
  //     "psolved_board.txt");
  return lv1Count * 10 + lv2Count * 20 + lv3Count * 50;
}

bool solveWithMediumStrategy(Board& board) {
  CandidatesBoard c_board = CandidatesBoard::fromBoard(board);
  while (true) {
    if (tryEasyStrategy1(board, c_board)) {
      continue;
    }

    if (tryEasyStrategy2(board, c_board)) {
      continue;
    }

    if (tryMediumStrategy1(board, c_board)) {
      continue;
    }

    if (tryMediumStrategy2(board, c_board)) {
      continue;
    }
    break;
  }
  return board.emptyCellsCount() == 0;
}

bool solveWithEasyStrategy(Board& board) {
  CandidatesBoard c_board = CandidatesBoard::fromBoard(board);
  while (true) {
    if (tryEasyStrategy1(board, c_board)) {
      continue;
    }

    if (tryEasyStrategy2(board, c_board)) {
      continue;
    }
    break;
  }
  return board.emptyCellsCount() == 0;
}

}  // namespace sk
