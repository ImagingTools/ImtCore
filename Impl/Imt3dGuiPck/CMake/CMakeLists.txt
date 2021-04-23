project(Imt3dGuiPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${ACFSLNDIR}/Config/CMake/AcfSln.cmake)

target_link_libraries(${PROJECT_NAME} imt3d imt3dgui imt3dview)

include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${ACFDIR}/Config/CMake/AcfStd.cmake)