project(ControlsGalleryGqlPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${ACFDIR}/Config/CMake/AcfStdGui.cmake)

link_directories(${IMTCOREDIR}/Lib/${CMAKE_BUILD_TYPE}_${TARGETNAME})
target_link_libraries(${PROJECT_NAME} controlsgallerygql controlsgallerysdl imtservergql imtserverapp imtgql imtcol imtbase imtbasesdl)


