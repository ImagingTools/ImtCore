project(Imt3dGuiPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${ACFDIR}/Config/CMake/AcfStd.cmake)
include(${ACFDIR}/Config/CMake/AcfQt.cmake)

target_link_libraries(${PROJECT_NAME} imt3d imt3dgui imt3dview imtgui imtwidgets imtstyle)
target_link_libraries(${PROJECT_NAME} imeas)