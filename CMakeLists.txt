cmake_minimum_required(VERSION 3.14)

project(spr332v2-utils
  LANGUAGES CXX
)

# default to a release build
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING
    "Build type. Options are: None Debug Release RelWithDebInfo MinSizeRel."
    FORCE
  )
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)
pkg_search_module(PCSC REQUIRED IMPORTED_TARGET libpcsclite)

add_library(spr332v2 OBJECT
  spr332v2.cpp
)
target_link_libraries(spr332v2 PUBLIC PkgConfig::PCSC)

add_executable(spr-nvram
  nvram.cpp
)
target_link_libraries(spr-nvram PRIVATE spr332v2)

add_executable(spr-buzzer
  buzzer.cpp
)
target_link_libraries(spr-buzzer PRIVATE spr332v2)

add_executable(spr-leds
  leds.cpp
)
target_link_libraries(spr-leds PRIVATE spr332v2)

