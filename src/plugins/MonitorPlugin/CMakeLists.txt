
set(PROJECT_MONITOR Omicron-Remote-Control_monitor)
project(${PROJECT_MONITOR} LANGUAGES CXX)

set(SRC_FILES
        src/MonitorPlugin.cpp
        src/MonitorPlugin.hpp)

if(UNIX AND NOT APPLE)
    set(SRC_FILES ${SRC_FILES}
            src/linux/Monitor_Linux.cpp
            src/linux/Monitor_Linux.hpp)

elseif(WIN32)
    set(SRC_FILES ${SRC_FILES}
            rc/monitorPlugin.rc
            src/windows/Monitor_Win32.cpp
            src/windows/Monitor_Win32.hpp)
endif()

add_library(${PROJECT_MONITOR} SHARED ${SRC_FILES})

target_link_libraries(${PROJECT_MONITOR} PRIVATE
        Qt5::Core
        ${PROJECT_CORE})

if(WIN32)
    target_link_libraries(${PROJECT_MONITOR} PRIVATE User32)
endif()
