cmake_minimum_required(VERSION 3.16)
project(Imt3dDemo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick ShaderTools)

add_executable(${PROJECT_NAME}
    main.cpp
    imt3ddemo.qrc
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::ShaderTools
    imt3dgui
    imt3dview
)
