cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
set(CMAKE_C_COMPILER "avr-gcc")
set(CMAKE_CXX_COMPILER "avr-g++")
add_link_options(-use-linker-plugin)
set(CMAKE_AR "avr-gcc-ar")
set(CMAKE_RANLIB "avr-gcc-ranlib")
set(CMAKE_C_ARCHIVE_CREATE "<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_C_ARCHIVE_APPEND "<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_CXX_ARCHIVE_CREATE "<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_CXX_ARCHIVE_APPEND "<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>")
project(NIM-Counter-firmware VERSION 0.1 DESCRIPTION "Firmware for the Open-NIM Counter modules." HOMEPAGE_URL "https://github.com/Open-NIM" LANGUAGES CXX C)

set(MCU "atmega2560")
set(ARDUINO "ARDUINO_AVR_MEGA2560")
set(SECTION_START "3E000")
set(BOOTLOADER "stk500boot")
set(NIM ${PROJECT_NAME})

set(MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
# set(CMAKE_VERBOSE_MAKEFILE True)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

list(APPEND CMAKE_C_FLAGS "-g -Os -w -ffunction-sections -fdata-sections -MMD -flto -fno-fat-lto-objects -mmcu=${MCU} -Wl,--gc-sections")
list(APPEND CMAKE_CXX_FLAGS "-g -Os -w -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -Wno-error=narrowing -MMD -flto -mmcu=${MCU}")
add_definitions(-DF_CPU=16000000L -DARDUINO=10819 -D${ARDUINO} -DARDUINO_ARCH_AVR -DU8G2_WITHOUT_FONT_ROTATION)


add_subdirectory(src)
add_executable(${NIM} src/NIM-Counter.cpp)
target_include_directories(${NIM} PUBLIC "${PROJECT_SOURCE_DIR}/include" "${PROJECT_SOURCE_DIR}/include/U8g2" "${PROJECT_SOURCE_DIR}/include/SPI" "${PROJECT_SOURCE_DIR}/include/wire" "${PROJECT_SOURCE_DIR}/include/U8g2/clib" "${PROJECT_SOURCE_DIR}/include/arduino")
target_link_libraries(${NIM} arduino wires spi U8g2)

unset(CMAKE_C_FLAGS)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS " -gdwarf-2 -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -fno-jump-tables -lm -MD -MP -MF -Wall -Wstrict-prototypes -Wl,--section-start=.text=E000 -mmcu=atmega2560 ")
add_executable(${BOOTLOADER} ${BOOTLOADER}-bootloader/${BOOTLOADER}.c)
# add_custom_command(
# 	TARGET NIM-Counter
# 	POST_BUILD
# 	COMMAND avr-objcopy ARGS -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 NIM-Counter ${PROJECT_NAME}.eep
# )
add_custom_command(
	TARGET ${NIM}
	POST_BUILD
	COMMAND avr-objcopy ARGS -O ihex -R .eeprom ${NIM} ${PROJECT_NAME}.hex
)
add_custom_command(
	TARGET ${BOOTLOADER}
	POST_BUILD
	COMMAND avr-objcopy ARGS -O ihex -R .eeprom ${BOOTLOADER} ${BOOTLOADER}.hex
)
add_custom_command(
	TARGET ${NIM} ${BOOTLOADER}
	POST_BUILD
	COMMAND echo ARGS ":020000040000FA" > ${PROJECT_NAME}.with-bootloader.tmp.hex
	COMMAND head ARGS -n -1 ${PROJECT_NAME}.hex >> ${PROJECT_NAME}.with-bootloader.tmp.hex 
	COMMAND echo ARGS ":020000040003F7" >>  ${PROJECT_NAME}.with-bootloader.tmp.hex
	COMMAND head ARGS -n -2 ${BOOTLOADER}.hex >> ${PROJECT_NAME}.with-bootloader.tmp.hex
	COMMAND echo ARGS ":040000030003E00019" >> ${PROJECT_NAME}.with-bootloader.tmp.hex
	COMMAND echo ARGS ":00000001FF" >> ${PROJECT_NAME}.with-bootloader.tmp.hex
	COMMAND sed  ARGS 's/\r//' ${PROJECT_NAME}.with-bootloader.tmp.hex > ${PROJECT_NAME}.with-bootloader.hex
)

set_property(
	TARGET ${NIM} ${BOOTLOADER}
	APPEND
	PROPERTY ADDITIONAL_CLEAN_FILES ${PROJECT_NAME}.with-bootloader.tmp.hex ${PROJECT_NAME}.with-bootloader.hex ${PROJECT_NAME}.hex ${BOOTLOADER}.hex
)
