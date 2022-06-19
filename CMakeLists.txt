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
find_package(PkgConfig)
pkg_check_modules(FFTW3 REQUIRED fftw3)

####### FFT #######
set(FFT_SOURCES
  src/maths/kiss_fft.c
  src/maths/kiss_fftr.c
)
set_source_files_properties(${FFT_SOURCES} PROPERTIES LANGUAGE CXX )
set(FFT_HEADERS
  include/maths/_kiss_fft_guts.h
  include/maths/kiss_fft.h
  include/maths/kiss_fftr.h
)

add_library(fft)
target_sources(fft
  PRIVATE ${FFT_SOURCES}
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
  PRIVATE ${DETECTION_FUNCTION_SOURCES}
  PUBLIC ${DETECTION_FUNCTION_HEADERS}
)
target_include_directories(detection_function PRIVATE 
  include
  include/maths
)
target_link_libraries(detection_function
  phase_vocoder)

set(TEMPO_TRACKV2_SOURCES
  src/bpmdetection/TempoTrackV2.cpp
)
set(TEMPO_TRACKV2_HEADERS
  include/bpmdetection/TempoTrackV2.h
)
add_library(tempo_track_v2)
target_sources(tempo_track_v2
  PRIVATE  ${TEMPO_TRACKV2_SOURCES}
  PUBLIC ${TEMPO_TRACKV2_HEADERS}
)
target_include_directories(tempo_track_v2 PRIVATE 
  include
  include/maths
)
target_link_libraries(tempo_track_v2
  phase_vocoder)

set(BUFFERING_UTILS_SOURCES
  src/bpmdetection/buffering_utils.cpp
  src/util/sample.cpp
  src/util/fpclassify.cpp
)
set(BUFFERING_UTILS_HEADERS
  include/bpmdetection/buffering_utils.hpp
  include/util/assert.h
  include/util/fpclassify.h
  include/util/math.h
  include/util/sample.h
)
add_library(buffering_utils)
target_sources(buffering_utils
  PRIVATE  ${BUFFERING_UTILS_SOURCES}
  PUBLIC ${BUFFERING_UTILS_HEADERS}
)
target_include_directories(buffering_utils PRIVATE 
  include
  include/util
  include/bpmdetection
)
set_target_properties(buffering_utils PROPERTIES LINKER_LANGUAGE CXX)

set(ANALYZER_QUEEN_MARY_BEATS_SOURCES
  src/bpmdetection/analyzerqueenmarybeats.cpp
)
set(ANALYZER_QUEEN_MARY_BEATS_HEADERS
  include/bpmdetection/constants.h
  include/bpmdetection/analyzerplugin.hpp
  include/bpmdetection/analyzerqueenmarybeats.hpp
)

add_library(analyzerqueenmarybeats)
target_sources(analyzerqueenmarybeats
  PRIVATE  ${ANALYZER_QUEEN_MARY_BEATS_SOURCES}
  PUBLIC ${ANALYZER_QUEEN_MARY_BEATS_HEADERS}
)
target_include_directories(analyzerqueenmarybeats PRIVATE 
  include
  include/util
  include/bpmdetection
)
target_link_libraries(analyzerqueenmarybeats
  phase_vocoder
  tempo_track_v2
  buffering_utils
  detection_function
)
####### Beatdetection BeatThis #######
set(FILTERBANK_SOURCES
  src/beatthis/filterbank.cpp)

set(FILTERBANK_HEADERS
  include/beatthis/filterbank.hpp)

set(FILTERBANK_TEST_SOURCES
  src/test/filterbank_test.cpp)

set(ENVELOPE_EXTRACTOR_SOURCES
  src/beatthis/envelope_extractor.cpp)

set(ENVELOPE_EXTRACTOR_HEADERS
  include/beatthis/envelope_extractor.hpp)

set(ENVELOPE_EXTRACTOR_TEST_SOURCES
  src/test/envelope_extractor_test.cpp)

set(DIFFERECTIFIER_SOURCES
  src/beatthis/differectifier.cpp)

set(DIFFERECTIFIER_HEADERS
  include/beatthis/differectifier.hpp)

set(DIFFERECTIFIER_TEST_SOURCES
  src/test/differectifier_test.cpp)

set(COMBFILTER_SOURCES
  src/beatthis/combfilter.cpp)

set(COMBFILTER_HEADERS
  include/beatthis/combfilter.hpp)

add_library(filterbank)
target_sources(filterbank
  PRIVATE  ${FILTERBANK_SOURCES}
  PUBLIC ${FILTERBANK_HEADERS}
)
target_include_directories(filterbank 
PRIVATE 
  include
  ${FFTW3_INCLUDE_DIRS}
)
target_link_libraries(filterbank
PRIVATE
  ${FFTW3_LIBRARIES}
)

add_executable(filterbank_test)
target_sources(filterbank_test
  PRIVATE ${FILTERBANK_TEST_SOURCES})
target_include_directories(filterbank_test
PRIVATE
  include
)
target_link_libraries(filterbank_test
PRIVATE
  filterbank
)

add_library(envelope_extractor)
target_sources(envelope_extractor
  PRIVATE  ${ENVELOPE_EXTRACTOR_SOURCES}
  PUBLIC ${ENVELOPE_EXTRACTOR_HEADERS}
)
target_include_directories(envelope_extractor 
PRIVATE 
  include
  ${FFTW3_INCLUDE_DIRS}
)
target_link_libraries(envelope_extractor
PRIVATE
  ${FFTW3_LIBRARIES}
)

add_executable(envelope_extractor_test)
target_sources(envelope_extractor_test
  PRIVATE ${ENVELOPE_EXTRACTOR_TEST_SOURCES})
target_include_directories(envelope_extractor_test
PRIVATE
  include
)
target_link_libraries(envelope_extractor_test
PRIVATE
  envelope_extractor
)

add_library(differectifier)
target_sources(differectifier
  PRIVATE  ${DIFFERECTIFIER_SOURCES}
  PUBLIC ${DIFFERECTIFIER_HEADERS}
)
target_include_directories(differectifier 
PRIVATE 
  include
  ${FFTW3_INCLUDE_DIRS}
)

add_executable(differectifier_test)
target_sources(differectifier_test
  PRIVATE  ${DIFFERECTIFIER_TEST_SOURCES}
)
target_include_directories(differectifier_test
PRIVATE
  include
)
target_link_libraries(differectifier_test
PRIVATE
  envelope_extractor
  differectifier
)

add_library(combfilter)
target_sources(combfilter
  PRIVATE  ${COMBFILTER_SOURCES}
  PUBLIC ${COMBFILTER_HEADERS}
)
target_include_directories(combfilter 
PRIVATE 
  include
  ${FFTW3_INCLUDE_DIRS}
)
target_link_libraries(combfilter
PRIVATE
  ${FFTW3_LIBRARIES}
)
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
  analyzerqueenmarybeats
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
  ${Qt5Widgets_INCLUDE_DIRS}
)

target_link_libraries(bpm_tab_main
  PRIVATE
  Qt5::Core
  Qt5::Widgets
  qtjack::qtjack
  bpm_tab
)

add_executable(super_cicular_buffer_test)
target_sources(super_cicular_buffer_test
  PRIVATE src/test/super_circular_buffer_test.cpp
)
target_include_directories(super_cicular_buffer_test PRIVATE include)