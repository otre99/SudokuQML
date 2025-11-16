#include <chrono>
#include <iostream>

#include "sudoku_logic.h"

using namespace std;

const string folder =
    "/home/rccr/RBT/REPOS/OT/SudokuQML/build/Desktop_Qt_6_9_0-Debug/";

void dp(const string &title,
        const std::chrono::high_resolution_clock::time_point &t1,
        const std::chrono::high_resolution_clock::time_point &t2) {
  cout << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()
       << "ms\n";
}

bool isSubset(const sk::Board &a, const sk::Board &b) {
  for (int i = 0; i < 81; ++i) {
    if (a(i) && a(i) != b(i)) {
      return false;
    }
  }
  return true;
}

int main(int argc, char *argv[]) {
  auto beg = std::chrono::high_resolution_clock::now();
  const sk::Board board = sk::generateSolvableBoard(25, 0);
  board.toFile("save_board.txt");

  // sk::Board board;
  // board.fromFile(folder + "save_board.txt");

  sk::Board solvedBoard = board;
  sk::completeSolution(solvedBoard, 0);
  solvedBoard.toFile(folder + "solved_board.txt");

  auto end = std::chrono::high_resolution_clock::now();
  dp("generateSolvableBoard: ", beg, end);

  sk::Board tmp = board;
  beg = std::chrono::high_resolution_clock::now();
  int score = sk::findBoardDificultyLevel(tmp);
  end = std::chrono::high_resolution_clock::now();
  dp("findBoardDificultyLevel: ", beg, end);

  if (isSubset(tmp, solvedBoard)) {
    cout << "OK\n";
  } else {
    cout << "ERROR\n";
  }
  return 0;
}
