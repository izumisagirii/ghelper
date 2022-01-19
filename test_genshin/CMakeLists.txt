cmake_minimum_required(VERSION 3.5)

project(test_genshin LANGUAGES CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(workthread)
add_subdirectory(key_hook)
add_subdirectory(gwindow)
add_subdirectory(locatemap)

find_package(Qt5 COMPONENTS Widgets LinguistTools REQUIRED)

set(TS_FILES config/test_genshin_zh_CN.ts)


add_executable(test_genshin WIN32
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    ${TS_FILES}
    config/app_win32.rc
    )

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

target_link_libraries(test_genshin PRIVATE
    Qt5::Widgets
    workthread
    keyhook
    gwindow
    locate)

#qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})


