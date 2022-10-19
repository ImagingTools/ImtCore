cmake_minimum_required(VERSION 3.1.0)

project(EjExport LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /wd4127 /wd4250 /wd4347 /wd4355 /wd4365 /wd4481 /wd4505 /wd4510 /wd4511 /wd4512 /wd4548 /wd4571 /wd4619 /wd4625 /wd4626 /wd4640 /wd4702 /wd4710 /wd4820 /wd4826")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /MP /fp:fast")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /bigobj")

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /Qpar /Gy /Gw /FS /Zc:threadSafeInit-")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "${CMAKE_CXX_FLAGS_RELEASE_INIT} /Ot /Oi /Ob2 /GS-")



set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Xml PrintSupport)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Xml PrintSupport)

message("QT_VERSION ${QT_VERSION}")

add_compile_definitions(COMMON_LIBRARY)

set(PROJECT_SOURCES
		../common/ejcommon.cpp
                docx/ejconvertordocx.cpp
                docx/ejworddocument.cpp
                docx/ejworddocumenteditors.cpp
                pdf/ejconvertorpdf.cpp
                pdf/ejpdfdocument.cpp
                pdf/ejpdfdocumenteditor.cpp
                standart_function.cpp
                csv/ejcsvdocument.cpp
                csv/ejcsvdocumenteditor.cpp
                csv/ejconvertorcsv.cpp
                testDocxToUjf/ejbaseujfconvertor.cpp
                testDocxToUjf/ejbaseujfdocument.cpp
                testDocxToUjf/docxtoujf/ejdocxtoujfdocument.cpp
                testDocxToUjf/docxtoujf/docxtoujfdocumenteditors.cpp
                testDocxToUjf/ejdocxtoujfconvertor.cpp
                testDocxToUjf/xlsxtoujf/sharedStrings/sharedstrings.cpp
                testDocxToUjf/xlsxtoujf/sharedStrings/stringsexceldocumentreader.cpp
                testDocxToUjf/xlsxtoujf/sheetexceldocumentreader.cpp
                testDocxToUjf/xlsxtoujf/styles/models/borders.cpp
                testDocxToUjf/xlsxtoujf/styles/models/cellxfs.cpp
                testDocxToUjf/xlsxtoujf/styles/models/fill.cpp
                testDocxToUjf/xlsxtoujf/styles/models/font.cpp
                testDocxToUjf/xlsxtoujf/styles/stylesexcel.cpp
                testDocxToUjf/xlsxtoujf/styles/stylesexceldocumentreader.cpp
                testDocxToUjf/xlsxtoujf/xlsxtoujfdocument.cpp
                testDocxToUjf/ejxlsxtoujfconvertor.cpp
                xlsx/ejconvertorexcel.cpp
                xlsx/documents/ejsharedstringsdocument.cpp
                xlsx/documents/ejsheetdocument.cpp
                xlsx/documents/ejstylesdocument.cpp
                xlsx/ejexceldocumenteditors.cpp
                xlsx/ejutils.cpp
		../include/exportdoc.h
		../include/ejcommon.h
		docx/ejconvertordocx.h
		docx/ejworddocument.h
		docx/ejworddocumenteditors.h
		pdf/ejconvertorpdf.h
		pdf/ejpdfdocument.h
		pdf/ejpdfdocumenteditor.h
		standart_function.cpp
		standart_function.h
		csv/ejcsvdocument.h
		csv/ejcsvdocumenteditor.h
		csv/ejconvertorcsv.h
		testDocxToUjf/ejbaseujfconvertor.h
		testDocxToUjf/ejbaseujfdocument.h
		testDocxToUjf/docxtoujf/ejdocxtoujfdocument.h
		testDocxToUjf/docxtoujf/docxtoujfdocumenteditors.h
		testDocxToUjf/ejdocxtoujfconvertor.h
		testDocxToUjf/xlsxtoujf/sharedStrings/sharedstrings.h
		testDocxToUjf/xlsxtoujf/sharedStrings/stringsexceldocumentreaders.h
		testDocxToUjf/xlsxtoujf/sheetexceldocumentreaders.h
		testDocxToUjf/xlsxtoujf/styles/models/borders.h
		testDocxToUjf/xlsxtoujf/styles/models/cellxfs.h
		testDocxToUjf/xlsxtoujf/styles/models/fill.h
		testDocxToUjf/xlsxtoujf/styles/models/font.h
		testDocxToUjf/xlsxtoujf/styles/stylesexcel.h
		testDocxToUjf/xlsxtoujf/styles/stylesexceldocumentreaders.h
		testDocxToUjf/xlsxtoujf/xlsxtoujfdocument.h
		testDocxToUjf/ejxlsxtoujfconvertor.h
		xlsx/ejconvertorexcel.h
		xlsx/documents/ejsharedstringsdocument.h
		xlsx/documents/ejsheetdocument.h
		xlsx/documents/ejstylesdocument.h
		xlsx/ejexceldocumenteditors.h
		xlsx/ejutils.h
                mini_zipper/zipper.cpp
		mini_zipper/zipper.h
                mini_zipper/qtiocompressor.cpp
		mini_zipper/qtiocompressor.h
		ejexport.qrc
)

set_target_properties(${target} PROPERTIES AUTOMOC ON)
set_target_properties(${target} PROPERTIES AUTORCC ON)
set_target_properties(${target} PROPERTIES AUTOUIC ON)



include_directories(../include/)

add_subdirectory(mini_zipper/)
include_directories(mini_zipper/)
include_directories(zlib/)

add_library(${PROJECT_NAME} ${PROJECT_SOURCES})



target_compile_definitions(${PROJECT_NAME}
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(${PROJECT_NAME}
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Xml Qt${QT_VERSION_MAJOR}::PrintSupport)

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
	MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

#set(outlibdir "${PROJECT_SOURCE_DIR}/../libs/${CMAKE_BUILD_TYPE}_${TARGETNAME}")

#set_target_properties( ${PROJECT_NAME}
#	PROPERTIES
#	ARCHIVE_OUTPUT_DIRECTORY ${outlibdir}
#	LIBRARY_OUTPUT_DIRECTORY ${outlibdir}
#	RUNTIME_OUTPUT_DIRECTORY ${outlibdir}
#	POSITION_INDEPENDENT_CODE ON
#)

#if(QT_VERSION_MAJOR EQUAL 6)
#    qt_import_qml_plugins(${PROJECT_NAME})
#    qt_finalize_executable(${PROJECT_NAME})
#endif()
