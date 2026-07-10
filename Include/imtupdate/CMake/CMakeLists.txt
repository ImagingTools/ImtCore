project(imtupdate)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)

target_link_libraries(${PROJECT_NAME} ${ACF_QT_MODULE_LINK_SCOPE} Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Svg)

target_include_directories(${PROJECT_NAME} PRIVATE
	"${CMAKE_CURRENT_LIST_DIR}/../../../../3rdParty"
)

if(ANDROID)
	find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
	find_package(Qt${QT_VERSION_MAJOR} COMPONENTS AndroidExtras REQUIRED)
	target_link_libraries(${PROJECT_NAME} ${ACF_QT_MODULE_LINK_SCOPE} Qt${QT_VERSION_MAJOR}::AndroidExtras)
endif()




