project(imtqml LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME} ${ACF_QT_MODULE_LINK_SCOPE} Qt${QT_VERSION_MAJOR}::Quick)
target_link_libraries(${PROJECT_NAME} ${ACF_QT_MODULE_LINK_SCOPE} Qt${QT_VERSION_MAJOR}::Qml)
target_link_libraries(${PROJECT_NAME} ${ACF_QT_MODULE_LINK_SCOPE} Qt${QT_VERSION_MAJOR}::QuickWidgets)
target_link_libraries(${PROJECT_NAME} ${ACF_QT_MODULE_LINK_SCOPE} Qt${QT_VERSION_MAJOR}::Concurrent)
