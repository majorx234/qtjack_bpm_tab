cmake_minimum_required(VERSION 3.16)
project(qtjack_bpm_tab
  DESCRIPTION "Qt program to tab BPM in GUI and send in periodic intervall Midi messages"
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

####### FFT #######
set(FFT_SOURCES
  src/maths/kiss_fft.c
  src/maths/kiss_fftr.c
)
set(FFT_HEADERS
  include/maths/_kiss_fft_guts.h
  include/maths/kiss_fft.h
  include/maths/kiss_fftr.h
)

add_library(fft)
target_sources(fft
  PRIVATE  ${FFT_SOURCES}
  PUBLIC ${FFT_HEADERS}
)
target_include_directories(fft PRIVATE 
  include
  include/maths
)
set_target_properties(fft PROPERTIES LINKER_LANGUAGE CXX)

####### Beat Detection #######
set(PHASE_VOCODER_SOURCES
  src/maths/FFT.cpp
  src/maths/MathUtilities.cpp
  src/bpmdetection/PhaseVocoder.cpp
  )
set(PHASE_VOCODER_HEADERS
  include/bpmdetection/PhaseVocoder.h
  include/bpmdetection/Window.h
  include/maths/FFT.h
  include/maths/MathUtilities.h)

add_library(phase_vocoder)
target_sources(phase_vocoder
  PRIVATE  ${PHASE_VOCODER_SOURCES}
  PUBLIC ${PHASE_VOCODER_HEADERS}
)
target_include_directories(phase_vocoder PRIVATE 
  include
  include/beatdetection
  include/maths
)
target_link_libraries(phase_vocoder 
  fft)

set(DETECTION_FUNCTION_SOURCES
  src/bpmdetection/detection_function.cpp
)
set(DETECTION_FUNCTION_HEADERS
  include/bpmdetection/detection_function.hpp
)
add_library(detection_function)
target_sources(detection_function
  PRIVATE  ${DETECTION_FUNCTION_HEADERS}
  PUBLIC ${DETECTION_FUNCTION_HEADERS}
)
target_include_directories(detection_function PRIVATE 
  include
  include/maths
)
target_link_libraries(detection_function
  phase_vocoder)

####### BPM Tab #######
set(AVRG_QUEUE_SOURCES
  src/avrg_queue.cpp
)
set(AVRG_QUEUE_HEADERS
  include/avrg_queue.hpp
)

set(AVRG_QUEUE_TEST_SOURCES
  src/test/avrg_queue_test.cpp
)

set(WAVE_WIDGET_SOURCES
  src/wave_widget.cpp
  include/wave_widget.hpp
)

set(BPM_TAB_SOURCES
  src/bpm_tab.cpp
  include/bpm_tab.hpp
)

set(BPM_TAB_MAIN_SOURCES
  src/bpm_tab_main.cpp
)

set(WAVE_WIDGET_TEST_SOURCES
  include/test/main_window_test.hpp
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

add_library(avrg_queue)
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
)

add_executable(wave_widget_test)
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
add_library(bpm_tab)
target_sources(bpm_tab
  PRIVATE
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

add_executable(bpm_tab_main)
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

target_link_libraries(bpm_tab_main
  PRIVATE
  Qt5::Core
  Qt5::Widgets
  qtjack::qtjack
  bpm_tab
)
