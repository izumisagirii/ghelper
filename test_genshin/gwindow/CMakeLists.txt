set(OpenCV_DIR E:/source/opencv-build/install)
find_package(OpenCV REQUIRED)
find_package(Qt5 COMPONENTS Widgets LinguistTools REQUIRED)

include_directories( ${OpenCV_INCLUDE_DIRS} )
set(LIBRARY_OUTPUT_PATH   ${PROJECT_SOURCE_DIR}/bin)
add_library(gwindow SHARED
        gwindow.cpp
        )
target_include_directories(gwindow PUBLIC  ./  )
target_link_libraries(gwindow
    ${OpenCV_LIBS}
    Qt5::Widgets
    )
