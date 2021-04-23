project(ImtCorePck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${ACFSLNDIR}/Config/CMake/AcfSln.cmake)

target_link_libraries(${PROJECT_NAME} imtbase imeas iinsp iproc)

include(${ACFDIR}/Config/CMake/AcfQt.cmake)
include(${ACFDIR}/Config/CMake/AcfStd.cmake)
