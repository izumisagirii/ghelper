#set(OpenCV_DIR E:/source/opencv-build/install)
#find_package(OpenCV REQUIRED)
find_package(Qt5 COMPONENTS Widgets LinguistTools REQUIRED)

#include_directories( ${OpenCV_INCLUDE_DIRS} )
set(LIBRARY_OUTPUT_PATH   ${PROJECT_SOURCE_DIR}/bin)
add_library(keyhook SHARED
        hook.cpp
        )
target_include_directories(keyhook PUBLIC  ./  )
target_link_libraries(keyhook
    Qt5::Widgets)
   # ${OpenCV_LIBS})
