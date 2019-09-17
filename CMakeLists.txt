cmake_minimum_required(VERSION 3.5)

project(mgsca LANGUAGES CXX)

find_package(Qt5 5.8 COMPONENTS Core Gui Widgets REQUIRED)

add_executable(mgsca
  main.cpp
  main_window.cpp
  main_window.h
  main_window.ui
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
  )
