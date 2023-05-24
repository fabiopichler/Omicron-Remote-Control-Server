
project(${PROJECT_APP} LANGUAGES CXX)

set(SRC_FILES
        qrc/files.qrc
        src/main.cpp
        src/AboutDialog.cpp
        src/AboutDialog.hpp
        src/App.cpp
        src/App.hpp
        src/MainWindow.cpp
        src/MainWindow.hpp)

if(WIN32)
    set(SRC_FILES ${SRC_FILES} rc/app.rc)
endif()

add_executable(${PROJECT_APP} ${SRC_FILES})

target_link_libraries(${PROJECT_APP} PRIVATE
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        ${PROJECT_CORE})
