
set(PROJECT_VOLUME Omicron-Remote-Control_masterVolume)
project(${PROJECT_VOLUME} LANGUAGES CXX)

find_package(ALSA REQUIRED)

set(SRC_FILES
        src/MasterVolumePlugin.cpp
        src/MasterVolumePlugin.hpp)

if(UNIX AND NOT APPLE)
    set(SRC_FILES ${SRC_FILES}
            src/linux/MasterVolume_Alsa.cpp
            src/linux/MasterVolume_Alsa.hpp)

    include_directories(${ALSA_INCLUDE_DIRS})
elseif(WIN32)
    set(SRC_FILES ${SRC_FILES}
            rc/masterVolumePlugin.rc
            src/windows/AudioEndpointVolumeCallback.cpp
            src/windows/AudioEndpointVolumeCallback.hpp
            src/windows/MasterVolume_Win32.cpp
            src/windows/MasterVolume_Win32.hpp)
endif()

add_library(${PROJECT_VOLUME} SHARED ${SRC_FILES})

target_link_libraries(${PROJECT_VOLUME} PRIVATE
        Qt5::Core
        ${PROJECT_CORE})

if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_VOLUME} PRIVATE ${ALSA_LIBRARIES})
elseif(WIN32)
    target_link_libraries(${PROJECT_VOLUME} PRIVATE Ole32 uuid)
endif()
