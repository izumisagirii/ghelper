set(OpenCV_DIR E:/source/opencv-build/install)
find_package(OpenCV REQUIRED)
find_package(Qt5 COMPONENTS Widgets LinguistTools REQUIRED)

include_directories( ${OpenCV_INCLUDE_DIRS} )
set(LIBRARY_OUTPUT_PATH   ${PROJECT_SOURCE_DIR}/bin)
add_library(autofish SHARED
        autofish.cpp
        )
target_include_directories(autofish PUBLIC  ./  )
target_link_libraries(autofish
    Qt5::Widgets
    gwindow
    ${OpenCV_LIBS})
