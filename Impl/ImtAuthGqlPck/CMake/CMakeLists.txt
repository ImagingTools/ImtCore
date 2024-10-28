project(ImtAuthGqlPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

target_link_libraries(${PROJECT_NAME} imtauthsdl imtauthgql imtqml imtlic imtguigql imtgql imtdb imtbase iauth idoc imtservergql)
target_link_libraries(${PROJECT_NAME}  Qt${QT_VERSION_MAJOR}::Quick)

include(${ACFDIR}/Config/CMake/AcfQt.cmake)
