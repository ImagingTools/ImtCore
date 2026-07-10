project(ImtGuiGqlPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME} ${ACF_PACKAGE_LINK_SCOPE} AcfSln::imeas AcfSln::iproc AcfSln::iauth)
target_link_libraries(${PROJECT_NAME} ${ACF_PACKAGE_LINK_SCOPE} imtgui imtloggui imtbase imtwidgets Acf::i2d Acf::iqt2d imtstyle imtgql imtguigql imtdb imtauth imtservergql)

