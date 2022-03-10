cmake_minimum_required(VERSION 3.16)
project(qtjack-examples
  DESCRIPTION "examples for qtjack"
  LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${PROJECT_SOURCE_DIR}/cmake")  

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 

find_package(Jack REQUIRED)
find_package( Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Multimedia REQUIRED)
find_package( qtjack REQUIRED)

####### Attenuator #######
set(ATTENUATOR_SOURCES src/attenuator.cpp)

add_executable(attenuator 
    ${ATTENUATOR_SOURCES}
)

target_include_directories(attenuator SYSTEM
  PUBLIC ${JACK_INCLUDE_DIRS}
  qtjack::qtjack
)

target_link_libraries(attenuator PRIVATE 
    qtjack::qtjack
    Qt5::Core
    ${JACK_LIBRARIES}
)

####### MP3 Player #######
set(MP3PLAYER_SOURCES src/mp3player.cpp
                      src/mp3player_mainwindow.cpp
                      include/mp3player_mainwindow.h)

QT5_WRAP_UI(MP3PLAYER_MAINWINDOW_UISrcs 
            ${CMAKE_CURRENT_SOURCE_DIR}/src/mp3player_mainwindow.ui)

QT5_WRAP_CPP(MP3PLAYER_MOCrcs
            ${MP3PLAYER_SOURCES}
)

add_executable(mp3player 
    ${MP3PLAYER_SOURCES}
    ${MP3PLAYER_MOCrcs}
    ${MP3PLAYER_MAINWINDOW_UISrcs}
)

target_include_directories(mp3player PRIVATE include)
target_include_directories(mp3player PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(mp3player SYSTEM
  PUBLIC
  ${Qt5Core_INCLUDE_DIRS}
  ${Qt5Widgets_INCLUDE_DIRS}
  ${Qt5Multimedia_ICLUDE_DIRS} 
  ${JACK_INCLUDE_DIRS}
  qtjack::qtjack
)

target_link_libraries(mp3player PRIVATE 
    qtjack::qtjack
    Qt5::Core
    Qt5::Widgets
    Qt5::Multimedia
    ${JACK_LIBRARIES}
)

####### Mididial #######
set(MIDIDIAL_SOURCES src/mididial.cpp)

QT5_WRAP_UI(MIDIDIAL_UISrcs 
            ${CMAKE_CURRENT_SOURCE_DIR}/src/mididial.ui)

add_executable(mididial
    ${MIDIDIAL_SOURCES}
    ${MIDIDIAL_UISrcs}
)
target_include_directories(mididial PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(mididial SYSTEM
  PUBLIC 
  ${Qt5Core_INCLUDE_DIRS}
  ${Qt5Widgets_INCLUDE_DIRS}
  ${JACK_INCLUDE_DIRS}
  qtjack::qtjack
)

target_link_libraries(mididial PRIVATE 
    qtjack::qtjack
    Qt5::Core
    Qt5::Widgets
    ${JACK_LIBRARIES}
)