#include "sudokumodel.h"

#include <random>

#include "sudoku_logic.h"

SudokuModel::SudokuModel(QObject* parent) : QAbstractListModel(parent) {
  m_data.resize(81);  // 9x9 board
}

int SudokuModel::rowCount(const QModelIndex&) const { return m_data.size(); }

QVariant SudokuModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid() || index.row() >= m_data.size()) return {};

  const SudokuCellData& cell = m_data[index.row()];
  switch (role) {
    case ValueRole:
      return cell.value;
    case CandidatesRole:
      return QVariant::fromValue(cell.candidates);
    case FrozenRole:
      return cell.isFrozen;
    case CellColorRole: {
      const int row = (index.row() / 9) / 3;
      const int col = (index.row() % 9) / 3;
      return (3 * row + col) % 2 ? "#e0f7fa" : "#fde0dc";
    }
    case ErrorRole:
      return m_data[index.row()].value != m_solvedBoard(index.row());
    case HintRole:
      return cell.isHint;
  }
  return {};
}

bool SudokuModel::setData(const QModelIndex& index, const QVariant& value,
                          int role) {
  if (!index.isValid() || index.row() >= m_data.size()) return false;

  SudokuCellData& cell = m_data[index.row()];
  switch (role) {
    case ValueRole:
      cell.value = value.toInt();
      cell.candidates.fill(false);
      break;
    case CandidatesRole:
      cell.candidates = value.value<QList<bool>>();
      break;
    case FrozenRole:
      cell.isFrozen = value.toBool();
      break;
    case HintRole:
      cell.isHint = value.toBool();
      break;
    default:
      return false;
  }

  emit dataChanged(index, index, {role});
  return true;
}

Qt::ItemFlags SudokuModel::flags(const QModelIndex&) const {
  return Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QHash<int, QByteArray> SudokuModel::roleNames() const {
  return {{ValueRole, "value"},     {CandidatesRole, "candidates"},
          {FrozenRole, "isFrozen"}, {CellColorRole, "cellColor"},
          {HintRole, "isHint"},     {ErrorRole, "isError"}};
}

void SudokuModel::setCell(int index, int value, QList<bool> candidates,
                          bool frozen) {
  if (index < 0 || index >= m_data.size()) return;

  bool plus = (value == 0) && (m_data[index].value > 0);  // add an empty cell
  bool minus = (value > 0 && m_data[index].value == 0);   // remove empty cell
  emptyCells += (plus - minus);

  const bool error = value && value != m_solvedBoard(index);
  const bool needBoardUpdate =
      value > 0 && value != m_data[index].value && error == false;

  m_data[index].value = value;
  m_data[index].candidates =
      candidates.isEmpty() ? QList<bool>(10, false) : candidates;
  m_data[index].isFrozen = frozen;
  emit dataChanged(this->index(index), this->index(index));

  if (error) {
    m_errorCount += 1;
    emit errorCountChanged();
  }

  if (needBoardUpdate) {
    const auto cellIndices = this->relatedCellIndices(index, false);
    std::for_each(cellIndices.cbegin(), cellIndices.cend(), [&](int i) {
      QList<bool>& cds = m_data[i].candidates;
      if (cds[value]) {
        cds[value] = false;
        emit dataChanged(this->index(i), this->index(i));
      }
    });
  }

  if (emptyCells <= 0) {
    bool wellSolved = true;
    for (int i = 0; i < 81; ++i) {
      if (m_data[i].value != m_solvedBoard(i)) {
        wellSolved = false;
        break;
      }
    }

    if (wellSolved != m_success) {
      m_success = wellSolved;
      emit successChanged();
    }
  }
}

void SudokuModel::resetBoard() {
  beginResetModel();
  m_data.fill(SudokuCellData());
  endResetModel();
}

Q_INVOKABLE QString SudokuModel::generateNewGame(int level) {
  sk::Board solvableBoard;
  for (int i = 0; i < 10; ++i) {
    bool success;
    solvableBoard =
        sk::generateSolvableBoard(20, level, success, &m_solvedBoard);
    if (success) break;
  }
  // m_solvedBoard.fromFile(
  //     "/home/rccr/RBT/REPOS/OT/SudokuQML/build/Desktop_Qt_6_9_0-Debug/"
  //     "psolved_board.txt");

  // sk::Board rtmp = solvableBoard;
  // sk::findBoardDificultyLevel(rtmp);

  const sk::CandidatesBoard c_board =
      sk::CandidatesBoard::fromBoard(solvableBoard);

  beginResetModel();
  m_data.fill(SudokuCellData());
  int index = 0;
  for (auto&& d : m_data) {
    const int row = index / 9;
    const int col = index % 9;
    if (m_precalculateAnnotations) {
      const bool* cds = c_board(row, col);
      std::copy(cds, cds + 10, d.candidates.begin());
    }
    d.value = solvableBoard(row, col);
    d.isFrozen = d.value != 0;
    ++index;
  }
  endResetModel();

  // save solution
  if (m_failCount != 0) {
    m_failCount = 0;
    emit failCountChanged();
  }

  if (m_errorCount != 0) {
    m_errorCount = 0;
    emit errorCountChanged();
  }
  if (m_success == true) {
    m_success = false;
    emit successChanged();
  }

  if (level == 0) {
    return "Easy";
  } else if (level == 1) {
    return "Medium";
  } else {
    return "Hard";
  }
}

QList<int> SudokuModel::getCellsInConflict(int index, int value) {
  const auto cellIndices = this->relatedCellIndices(index, false);
  QList<int> result;
  std::copy_if(cellIndices.begin(), cellIndices.end(),
               std::back_insert_iterator<QList<int>>(result),
               [&](int i) { return m_data[i].value == value; });

  if (!result.empty()) {
    m_failCount += 1;
    emit failCountChanged();
  }
  return result;
}

QList<int> SudokuModel::relatedCellIndices(const int index,
                                           const bool include_index) const {
  QList<int> result;
  bool uniques[81] = {false};

  const int row = index / 9;
  const int col = index % 9;
  const int i0 = (row / 3) * 3;
  const int j0 = (col / 3) * 3;
  int k = 0;
  for (int i = i0; i < i0 + 3; ++i) {
    for (int j = j0; j < j0 + 3; ++j) {
      uniques[i * 9 + j] = true;
      uniques[row * 9 + k] = true;
      uniques[k * 9 + col] = true;
      ++k;
    }
  }
  uniques[index] = include_index;
  for (int n = 0; n < 81; ++n) {
    if (uniques[n]) result.push_back(n);
  }
  return result;
}

Q_INVOKABLE int SudokuModel::hint() {
  std::vector<int> indices(81);
  std::iota(indices.begin(), indices.end(), 0);
  shuffle(indices.begin(), indices.end(), std::mt19937(std::random_device()()));

  for (int index : indices) {
    if (m_data[index].value == 0) {
      const int hint_value = m_solvedBoard(index);
      m_data[index].value = hint_value;
      m_data[index].isFrozen = m_data[index].isHint = true;
      m_data[index].candidates.fill(false);
      emit dataChanged(this->index(index), this->index(index),
                       {ValueRole, CandidatesRole, HintRole});

      // update the rest
      const auto cellIndices = this->relatedCellIndices(index, false);
      std::for_each(cellIndices.cbegin(), cellIndices.cend(), [&](int i) {
        QList<bool>& cds = m_data[i].candidates;
        if (cds[hint_value]) {
          cds[hint_value] = false;
          emit dataChanged(this->index(i), this->index(i), {CandidatesRole});
        }
      });
      return m_solvedBoard(index);
    }
  }
  return 0;
}
