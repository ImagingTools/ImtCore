project(ImtGuiPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME} ${ACF_PACKAGE_LINK_SCOPE} AcfSln::imeas AcfSln::iproc AcfSln::iedge Acf::iqtdoc)
target_link_libraries(${PROJECT_NAME} ${ACF_PACKAGE_LINK_SCOPE} imtgui imtcol AcfSln::iauth imtloggui imtbase imtwidgets Acf::i2d Acf::iqt2d imtstyle)


