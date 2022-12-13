file(GLOB arduinoSRC CONFIGURE_DEPENDS "arduino/*.cpp" "arduino/*.c")
file(GLOB U8g2SRC CONFIGURE_DEPENDS "U8g2/clib/*")

add_library(arduino ${arduinoSRC})
target_include_directories(arduino PUBLIC "${PROJECT_SOURCE_DIR}/include/arduino" "${PROJECT_SOURCE_DIR}/include")

add_library(spi SPI/SPI.cpp)
target_include_directories(spi PUBLIC "${PROJECT_SOURCE_DIR}/include/SPI")
target_link_libraries(spi arduino)

add_subdirectory(wire)

add_library(U8g2 ${U8g2SRC} U8g2/U8g2lib.cpp U8g2/U8x8lib.cpp )
target_include_directories(U8g2 PUBLIC "${PROJECT_SOURCE_DIR}/include/U8g2" "${PROJECT_SOURCE_DIR}/include/SPI" "${PROJECT_SOURCE_DIR}/include/wire" "${PROJECT_SOURCE_DIR}/include/U8g2/clib")
target_link_libraries(U8g2 arduino spi wires )



# add_executable(NIM-Counter ${arduinoSRC} ${U8g2SRC} U8g2/U8g2lib.cpp U8g2/U8x8lib.cpp SPI/SPI.cpp wire/Wire.cpp wire/utility/twi.c main.cpp)
# target_include_directories(NIM-Counter PUBLIC "${PROJECT_SOURCE_DIR}/include" "${PROJECT_SOURCE_DIR}/include/U8g2" "${PROJECT_SOURCE_DIR}/include/SPI" "${PROJECT_SOURCE_DIR}/include/wire" "${PROJECT_SOURCE_DIR}/include/U8g2/clib" "${PROJECT_SOURCE_DIR}/include/arduino" "${PROJECT_SOURCE_DIR}/include/wire/utility")

