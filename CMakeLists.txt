cmake_minimum_required(VERSION 3.16)
project(qtjack-examples
  DESCRIPTION "examples for qtjack"
  LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${PROJECT_SOURCE_DIR}/cmake")  

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 

find_package(Jack REQUIRED)
find_package( Qt5Core REQUIRED)
find_package( qtjack REQUIRED)

set(ATTENUATOR_SOURCES src/attenuator.cpp)

#QT5_WRAP_CPP(ATTENUATOR_MOCrcs ${ATTENUATOR_SOURCES})

add_executable(attenuator 
    ${ATTENUATOR_SOURCES}
#    ${ATTENUATOR_MOCrcs}
)

target_include_directories(attenuator SYSTEM
  PUBLIC ${JACK_INCLUDE_DIRS})

message("test ${JACK_LIBRARIES}")
target_link_libraries(attenuator PRIVATE 
    qtjack::qtjack
    Qt5::Core
    ${JACK_LIBRARIES}
    /usr/lib/libjackserver.so
)
