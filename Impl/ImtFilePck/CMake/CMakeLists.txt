project(ImtFilePck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME} imtbase imtfile) # imeas iinsp iproc imtlog iauth)

include(${ACFDIR}/Config/CMake/AcfQt.cmake)
