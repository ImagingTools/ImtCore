project(imtgui LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

set(DESIGN_TOKEN_CREATOR_COMMAND_PARAM_IMAGES_INPUT_DIR "${PROJECT_SOURCE_DIR}/../Resources/Icons/Template")
set(DESIGN_TOKEN_CREATOR_COMMAND_PARAM_STYIES_INPUT_DIR "-")
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

ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-D_UNICODE)
