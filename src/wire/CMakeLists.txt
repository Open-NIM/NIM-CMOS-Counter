
# set(CMAKE_C_COMPILER "avr-gcc")
# set(CMAKE_CXX_COMPILER "avr-g++")
# add_definitions(-DF_CPU=16000000L -DARDUINO=10819 -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR -D__AVR_ATmega2560__)

add_library(wires Wire.cpp utility/twi.c)
target_include_directories(wires PUBLIC "${PROJECT_SOURCE_DIR}/include/wire" "${PROJECT_SOURCE_DIR}/include/wire/utility" "${PROJECT_SOURCE_DIR}/include/arduino" "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(wires arduino )
