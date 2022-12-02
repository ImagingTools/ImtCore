cmake_minimum_required(VERSION 3.1.0)

project(mini_zipper LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(${MSVC})
   set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /wd4127 /wd4250 /wd4347 /wd4355 /wd4365 /wd4481 /wd4505 /wd4510 /wd4511 /wd4512 /wd4548 /wd4571 /wd4619 /wd4625 /wd4626 /wd4640 /wd4702 /wd4710 /wd4820 /wd4826")
   set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /MP /fp:fast")
   set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS")
   set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /bigobj")

   set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /Qpar /Gy /Gw /FS /Zc:threadSafeInit-")
   set(CMAKE_CXX_FLAGS_RELEASE_INIT "${CMAKE_CXX_FLAGS_RELEASE_INIT} /Ot /Oi /Ob2 /GS-")
endif()



set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

message("QT_VERSION ${QT_VERSION}")

add_compile_definitions(COMMON_LIBRARY)



set(PROJECT_SOURCES
#                zipper.cpp
		zipper.h
#                qtiocompressor.cpp
		qtiocompressor.h
)

include_directories(../zlib/)

add_library(${PROJECT_NAME} ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Core)
