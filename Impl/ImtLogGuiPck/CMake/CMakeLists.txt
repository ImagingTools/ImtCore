project(ImtLogGuiPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${ACFDIR}/Config/CMake/AcfStd.cmake)

target_link_libraries(${PROJECT_NAME} imeas iinsp iipr iproc icalib imtgui imtlog imtloggui imtbase imtwidgets i2d iqt2d iedge)

include(${ACFDIR}/Config/CMake/AcfQt.cmake)