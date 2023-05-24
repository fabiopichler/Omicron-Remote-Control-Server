
project(${PROJECT_CORE} LANGUAGES CXX)

add_definitions(-DORC_BUILD_CORE_LIB)

set(SRC_FILES
        src/private/BluetoothServer.cpp
        src/private/ClientSocket.hpp
        src/private/ClientSocketList.cpp
        src/private/PluginManager.cpp
        src/private/ServerManager.cpp
        src/private/TcpServer.cpp
        src/Server.cpp
        ../../include/core/Server.hpp
        src/PluginBase.cpp
        ../../include/core/PluginBase.hpp
        src/Bluetooth.cpp
        ../../include/core/Bluetooth.hpp
        src/Config.cpp
        ../../include/core/Config.hpp)

if(WIN32)
    set(SRC_FILES ${SRC_FILES} rc/core.rc)
endif()

add_library(${PROJECT_CORE} SHARED ${SRC_FILES})

target_link_libraries(${PROJECT_CORE} PRIVATE
        Qt5::Core
        Qt5::Network
        Qt5::Bluetooth)
