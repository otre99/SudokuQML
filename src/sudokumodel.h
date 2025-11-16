#ifndef SUDOKUMODEL_H
#define SUDOKUMODEL_H

#include <qqmlintegration.h>

#include <QAbstractListModel>

struct SudokuCellData {
  int value = 0;
  QList<bool> candidates = QList<bool>(10, false);
  bool isFrozen = false;
  bool isHint = false;
};

#include "sudoku_logic.h"

class SudokuModel : public QAbstractListModel {
  Q_OBJECT
  QML_SINGLETON
  QML_ELEMENT
  Q_PROPERTY(int failCount READ failCount NOTIFY failCountChanged)
  Q_PROPERTY(int errorCount READ errorCount NOTIFY errorCountChanged)
  Q_PROPERTY(int success READ success NOTIFY successChanged)

 public:
  enum Roles {
    ValueRole = Qt::UserRole + 1,
    CandidatesRole,
    FrozenRole,
    CellColorRole,
    HintRole,
    ErrorRole
  };

  explicit SudokuModel(QObject* parent = nullptr);

  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant data(const QModelIndex& index, int role) const override;

  bool setData(const QModelIndex& index, const QVariant& value,
               int role) override;
  QHash<int, QByteArray> roleNames() const override;
  Qt::ItemFlags flags(const QModelIndex& index) const override;

  Q_INVOKABLE void setCell(int index, int value, QList<bool> candidates = {},
                           bool frozen = false);
  void resetBoard();
  Q_INVOKABLE QString generateNewGame(int level);
  Q_INVOKABLE QList<int> getCellsInConflict(int index, int value);
  Q_INVOKABLE int hint();
  Q_INVOKABLE void precalculateAnnotations(bool p) {
    m_precalculateAnnotations = p;
  }

  int failCount() const { return m_failCount; }
  int errorCount() const { return m_errorCount; }
  int success() const { return m_success; }
 signals:
  void failCountChanged();
  void errorCountChanged();
  void successChanged();

 private:
  sk::Board m_solvedBoard;
  QVector<SudokuCellData> m_data;
  QList<int> relatedCellIndices(const int index,
                                const bool include_index) const;
  int m_failCount{0};
  int m_errorCount{0};
  bool m_precalculateAnnotations{false};
  bool m_success;
  int emptyCells{0};
};

#endif  // SUDOKUMODEL_H
