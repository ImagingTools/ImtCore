project(ImtLogGuiPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME} ${ACF_PACKAGE_LINK_SCOPE} AcfSln::imeas AcfSln::iinsp AcfSln::iipr AcfSln::iproc AcfSln::icalib imtgui imtloggui imtlog imtbase imtwidgets Acf::i2d Acf::iqt2d AcfSln::iedge)


