cmake_minimum_required(VERSION 3.19)

if(WIN32)
        set(XLSX_LIB_SEARCH_PATH "${CMAKE_CURRENT_LIST_DIR}/../../3rdParty/QXlsx/Lib/${CMAKE_BUILD_TYPE}${TARGETNAME}")
else()
        set(XLSX_LIB_SEARCH_PATH "${CMAKE_CURRENT_LIST_DIR}/../../3rdParty/QXlsx/Lib/${TARGETNAME}")
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../../3rdParty/QXlsx/header)

find_library(
        XLSX_LIB_PATH
        QXlsx
        ${XLSX_LIB_SEARCH_PATH}
)

message("QXlsx XLSX_LIB_PATH ${XLSX_LIB_PATH} XLSX_LIB_SEARCH_PATH ${XLSX_LIB_SEARCH_PATH}")


