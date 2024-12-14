cmake_minimum_required(VERSION 3.10)

project(DMSSoft)

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-g")

find_package(ROOT REQUIRED COMPONENTS RIO Hist Gui)
include_directories(${ROOT_INCLUDE_DIRS})
link_directories(${ROOT_LIBRARY_DIRS})
add_definitions(${ROOT_CXX_FLAGS})

# Boost config
set(BOOST_ROOT "/cluster/home/wyjang/app/boost/1.87.0/x86_64-sl7-gcc8.1.0")
find_package(Boost REQUIRED COMPONENTS program_options)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

# 소스 파일
set(SOURCES
  main.cxx
  DMSPlotter.cxx
  DMSMathUtils.cxx
  ${CMAKE_CURRENT_BINARY_DIR}/DMSSoft_Dict.cxx
  DMSDataProcess.cxx
  LinkDef.h
)

# Dictionary 파일 생성 규칙
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/DMSSoft_Dict.cxx ${CMAKE_CURRENT_BINARY_DIR}/DMSSoft_Dict_rdict.pcm
  COMMAND rootcling -f ${CMAKE_CURRENT_BINARY_DIR}/DMSSoft_Dict.cxx -c
  ${CMAKE_CURRENT_SOURCE_DIR}/DMSPlotter.h
  ${CMAKE_CURRENT_SOURCE_DIR}/DMSMathUtils.h
  ${CMAKE_CURRENT_SOURCE_DIR}/DMSDataProcess.h
  ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef.h
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/DMSPlotter.h
  ${CMAKE_CURRENT_SOURCE_DIR}/DMSMathUtils.h
  ${CMAKE_CURRENT_SOURCE_DIR}/DMSDataProcess.h
  ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef.h
)

# 실행 파일 생성
add_executable(dmssoft ${SOURCES})

# 링크할 라이브러리
target_link_libraries(dmssoft ${ROOT_LIBRARIES} Boost::program_options)

# 클린 규칙
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "DMSSoft_Dict.cxx;DMSSoft_Dict_rdict.pcm")
