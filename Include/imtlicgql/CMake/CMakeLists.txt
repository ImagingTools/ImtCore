project(imtlicgql LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME} imtguigql imtbase imtlic imtgui imtwidgets imtauth)

find_package(QT NAMES Qt6 Qt5 REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets LinguistTools REQUIRED)

set(TS_FILES
	Lisa_ru_RU.ts
)

qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

message("TEST MESSAGE")
foreach(_ts_file ${TS_FILES})
	message("$${Qt5_LUPDATE_EXECUTABLE} -recursive ${CMAKE_CURRENT_SOURCE_DIR} -ts ${_ts_file}")
#	add_custom_command(
#		COMMENT "LUPDATE EXEC"
#		OUTPUT "${_ts_file}"
#		DEPENDS ${TS_SOURCES}
#		COMMAND ${Qt5_LUPDATE_EXECUTABLE} -recursive ${CMAKE_CURRENT_SOURCE_DIR} -ts ${_ts_file}
#	)

    add_custom_target(lupdate
		COMMAND ${Qt${QT_VERSION_MAJOR}_LUPDATE_EXECUTABLE} -recursive ${PROJECT_SOURCE_DIR} -ts ${_ts_file}
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/../
		COMMENT "Updating translations"
	)
endforeach()

