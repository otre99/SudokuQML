#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "sudokumodel.h"

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);

  app.setApplicationName("SudokuQML");
  app.setOrganizationName("RCCR");
  app.setOrganizationDomain("rccr1987.com");

  QQmlApplicationEngine engine;
  auto *s = engine.singletonInstance<SudokuModel *>("Sudoku", "SudokuModel");
  /*initialization when necessary*/

  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
  engine.loadFromModule("Sudoku", "Main");

  return app.exec();
}
