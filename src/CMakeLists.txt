cmake_minimum_required(VERSION 3.16)

project(Sudoku VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appSudoku
    main.cpp
)

qt_add_qml_module(appSudoku
    URI Sudoku
    VERSION 1.0
    QML_FILES
        Main.qml
        SudokuCell.qml
        SetterButton.qml
        GlobalsUtil.qml
        TopBar.qml
        SettingsUI.qml
        GameSummary.qml
    SOURCES
        sudokumodel.cpp sudokumodel.h
        sudokumodel.cpp sudokumodel.h
        sudoku_logic.cpp sudokumodel.h
)

target_link_libraries(appSudoku
    PRIVATE Qt6::Quick
)

include(GNUInstallDirs)
install(TARGETS appSudoku
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
