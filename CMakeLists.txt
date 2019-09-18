cmake_minimum_required(VERSION 3.5)

project(mgsca LANGUAGES CXX)

find_package(Qt5 5.8 COMPONENTS Core Gui Widgets REQUIRED)
find_package(OpenSSL 1.0 REQUIRED)

option(YAML_CPP_BUILD_TESTS "" OFF)
option(YAML_CPP_BUILD_TOOLS "" OFF)
option(YAML_CPP_BUILD_CONTRIB "" OFF)
option(YAML_CPP_INSTALL "" OFF)
add_subdirectory(yaml-cpp)

add_executable(mgsca
  main.cpp
  main_window.cpp
  main_window.h
  main_window.ui
  document.cpp
  document.h
  )

set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 11
  CXX_STANDARD_REQUIRED ON
  AUTOMOC ON
  AUTOUIC ON
  AUTORCC ON
  )

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )

target_link_libraries(mgsca PRIVATE
  Qt5::Core
  Qt5::Gui
  Qt5::Widgets
  OpenSSL::SSL
  OpenSSL::Crypto
  yaml-cpp
  )
