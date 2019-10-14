cmake_minimum_required(VERSION 3.5)

project(mgsca LANGUAGES CXX)

# Boring SSL
add_definitions(-DBORINGSSL_NO_STATIC_INITIALIZER)
add_subdirectory(boringssl)
add_library(boringssl INTERFACE)
target_link_libraries(boringssl INTERFACE
  ssl
  crypto
  )
target_include_directories(boringssl INTERFACE
  boringssl/include
  )

# yaml-cpp
option(YAML_CPP_BUILD_TESTS "" OFF)
option(YAML_CPP_BUILD_TOOLS "" OFF)
option(YAML_CPP_BUILD_CONTRIB "" OFF)
option(YAML_CPP_INSTALL "" OFF)
add_subdirectory(yaml-cpp)

find_package(Qt5 5.8 COMPONENTS Core Gui Widgets REQUIRED)

add_executable(mgsca
  main.cpp
  main_window.cpp
  main_window.h
  main_window.ui
  idle_handler.cpp
  idle_handler.h
  idle_handler_p.h
  document.cpp
  document.h
  certificate_item.cpp
  certificate_item.h
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
  boringssl
  yaml-cpp
  )
