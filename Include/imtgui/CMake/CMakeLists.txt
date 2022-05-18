project(imtgui LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

set(DESIGN_TOKEN_CREATOR_COMMAND_PARAM_IMAGES_INPUT_DIR "${PROJECT_SOURCE_DIR}/../Resources/Icons/Template")
set(DESIGN_TOKEN_CREATOR_COMMAND_PARAM_STYIES_INPUT_DIR "${PROJECT_SOURCE_DIR}/../Resources/Styles/Template")
set(DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR "${AUX_INCLUDE_DIR}/${PROJECT_NAME}")
include(${CMAKE_CURRENT_SOURCE_DIR}/../../../Config/CMake/DesignTokenCreator.cmake)

list(APPEND QML_IMPORT_DIRS "${IMTCOREDIR}/Include")
list(REMOVE_DUPLICATES QML_IMPORT_DIRS)

# The variable is cached in ${BUILD_DIR}/CMakeCache.txt. We need FORCE to
# change it there immediately. Also, add a comment to the cache file.
set(QML_IMPORT_PATH ${QML_IMPORT_DIRS}
	CACHE STRING "Qt Creator 5.0 extra qml import paths"
	FORCE
)

message("imtgui QML_IMPORT_PATH ${QML_IMPORT_PATH}")

find_package(QT NAMES Qt6 Qt5 REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets LinguistTools REQUIRED)

set(TS_FILES
	Lisa_ru_RU.ts
	Lisa_en_US.ts
	Lisa_de_DE.ts
	Lisa_pl_pl.ts
)

qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

foreach(_ts_file ${TS_FILES})
	message("$${Qt5_LUPDATE_EXECUTABLE} -recursive ${CMAKE_CURRENT_SOURCE_DIR} -ts ${_ts_file}")

	add_custom_target(lupdate_${_ts_file}
		COMMAND ${Qt${QT_VERSION_MAJOR}_LUPDATE_EXECUTABLE} -recursive ${PROJECT_SOURCE_DIR}/../ -ts ${_ts_file}
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
		COMMENT "Updating translations"
	)
endforeach()



ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-D_UNICODE)
