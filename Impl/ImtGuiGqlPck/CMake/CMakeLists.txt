project(ImtGuiGqlPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME} imeas iproc iauth
target_link_libraries(${PROJECT_NAME} imtgui imtloggui imtbase imtwidgets i2d iqt2d imtstyle imtgql imtguigql imtdb)

include(${ACFDIR}/Config/CMake/AcfQt.cmake)
