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
find_package(Threads REQUIRED)
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
set(MIDIDIAL_SOURCES src/mididial.cpp
                     src/mididial_mainwindow.cpp
                     include/mididial_mainwindow.h 
)

QT5_WRAP_CPP(MIDIDIAL_MOCrcs
            ${MIDIDIAL_SOURCES}
)

QT5_WRAP_UI(MIDIDIAL_UISrcs 
            ${CMAKE_CURRENT_SOURCE_DIR}/src/mididial.ui)

add_executable(mididial
    ${MIDIDIAL_SOURCES}
    ${MIDIDIAL_UISrcs}
    ${MIDIDIAL_MOCrcs}
)
target_include_directories(mididial PRIVATE include)
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

####### Sinedial #######
set(SINEDIAL_SOURCES src/sinedial.cpp
                     src/sinedial_mainwindow.cpp
                     include/sinedial_mainwindow.h
)

QT5_WRAP_CPP(SINEDIAL_MOCrcs
            ${SINEDIAL_SOURCES}
)

QT5_WRAP_UI(SINEDIAL_UISrcs
            ${CMAKE_CURRENT_SOURCE_DIR}/src/sinedial.ui)

add_executable(sinedial
    ${SINEDIAL_SOURCES}
    ${SINEDIAL_UISrcs}
    ${SINEDIAL_MOCrcs}
)
target_include_directories(sinedial PRIVATE include)
target_include_directories(sinedial PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(sinedial SYSTEM
  PUBLIC
  ${Qt5Core_INCLUDE_DIRS}
  ${Qt5Widgets_INCLUDE_DIRS}
  ${JACK_INCLUDE_DIRS}
  qtjack::qtjack
)

target_link_libraries(sinedial PRIVATE
    qtjack::qtjack
    Qt5::Core
    Qt5::Widgets
    ${JACK_LIBRARIES}
		sine_wave
)


####### BPM Tab #######
set(AVRG_QUEUE_SOURCES
  src/avrg_queue.cpp
)
set(AVRG_QUEUE_HEADERS
  include/avrg_queue.h
)

set(AVRG_QUEUE_TEST_SOURCES
  src/test/avrg_queue_test.cpp
)

set(SINE_WAVE_SOURCES
  src/sine_wave.cpp
	include/sine_wave.h
)

set(WAVE_WIDGET_SOURCES
	src/wave_widget.cpp
	include/wave_widget.h
)

set(BPM_TAB_SOURCES
  src/bpm_tab.cpp
  include/bpm_tab.h
)

set(BPM_TAB_MAIN_SOURCES
  src/bpm_tab_main.cpp
)

set(WAVE_WIDGET_TEST_SOURCES
	include/test/main_window_test.h
	src/test/main_window_test.cpp
  src/test/wave_widget_test.cpp
)

QT5_WRAP_CPP(WAVE_WIDGET_MOCrcs
  ${WAVE_WIDGET_SOURCES}
)


QT5_WRAP_CPP(BPM_TAB_MOCrcs
  ${BPM_TAB_SOURCES}
)

QT5_WRAP_UI(BPM_TAB_UISrcs 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/bpm_tab.ui
)

QT5_WRAP_CPP(WAVE_WIDGET_TEST_MOCrcs
  ${WAVE_WIDGET_TEST_SOURCES}
)

add_library(avrg_queue
  ${AVRG_QUEUE_SOURCES}
  ${AVRG_QUEUE_HEADERS}
)
target_sources(avrg_queue
  PRIVATE  ${AVRG_QUEUE_SOURCES}
  PUBLIC ${AVRG_QUEUE_HEADERS}
)
target_include_directories(avrg_queue PRIVATE include)

add_executable(avrg_queue_test
  ${AVRG_QUEUE_TEST_SOURCES}
)
target_sources(avrg_queue_test
  PRIVATE  ${AVRG_QUEUE_TEST_SOURCES}
)
target_include_directories(avrg_queue_test PRIVATE include)
target_link_libraries(avrg_queue_test
  avrg_queue
	)

add_library(sine_wave
	${SINE_WAVE_SOURCES})
target_include_directories(sine_wave PRIVATE include)

add_library(wave_widget
	${WAVE_WIDGET_SOURCES}  
	${WAVE_WIDGET_MOCrcs}  
	)
target_include_directories(wave_widget PRIVATE include)
target_include_directories(wave_widget PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(wave_widget SYSTEM
  PUBLIC 
  ${Qt5Core_INCLUDE_DIRS}
  ${Qt5Widgets_INCLUDE_DIRS}
)
target_link_libraries(wave_widget PRIVATE 
  Qt5::Core
  Qt5::Widgets
	sine_wave
)

add_executable(wave_widget_test
  ${WAVE_WIDGET_TEST_SOURCES}
)
target_sources(wave_widget_test
  PRIVATE  ${WAVE_WIDGET_TEST_SOURCES}
	${WAVE_WIDGET_TEST_MOCrcs}
)
target_include_directories(wave_widget_test PRIVATE include)
target_link_libraries(wave_widget_test
  wave_widget
	Qt5::Core
  Qt5::Widgets
)

add_library(bpm_tab
  ${BPM_TAB_SOURCES}
  ${BPM_TAB_UISrcs}
  ${BPM_TAB_MOCrcs}
)

QT5_WRAP_CPP(BPM_TAB_MAIN_MOCrcs
 ${BPM_TAB_MAIN_SOURCES}
)

target_include_directories(bpm_tab PRIVATE include)
target_include_directories(bpm_tab PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(bpm_tab SYSTEM
  PUBLIC 
  ${Qt5Core_INCLUDE_DIRS}
  ${Qt5Widgets_INCLUDE_DIRS}
  ${JACK_INCLUDE_DIRS}
  qtjack::qtjack
)

target_link_libraries(bpm_tab PRIVATE 
  qtjack::qtjack
  Qt5::Core
  Qt5::Widgets
  ${JACK_LIBRARIES}
  avrg_queue
	wave_widget
	Threads::Threads
)

add_executable(bpm_tab_main
  ${BPM_TAB_MAIN_MOCrcs}
  ${BPM_TAB_MAIN_SOURCES}
)

target_sources(bpm_tab_main
  PRIVATE ${BPM_TAB_MAIN_MOCrcs}
  PUBLIC ${BPM_TAB_MAIN_SOURCES}
)

target_include_directories(bpm_tab_main PRIVATE include)
target_include_directories(bpm_tab_main PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(bpm_tab_main SYSTEM
  PUBLIC
  qtjack::qtjack
  ${JACK_INCLUDE_DIRS}
  ${Qt5Core_INCLUDE_DIRS}
  ${Qt5Widgets_INCLUDE_DIRS}4
)

target_link_libraries(bpm_tab_main PRIVATE 
    Qt5::Core
    Qt5::Widgets
    qtjack::qtjack
    bpm_tab
)
