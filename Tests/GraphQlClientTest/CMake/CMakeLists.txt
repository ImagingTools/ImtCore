project(GraphQlClientTest LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ApplicationConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/OpenSSL.cmake)
include(${IMTCOREDIR}/Config/CMake/Quazip.cmake)

# find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Network)

# acf_create_moc(MOC_SOURCES "${PROJECT_SOURCE_DIR}/../Main.cpp")

target_link_libraries(${PROJECT_NAME} graphqlclient)
target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::QuickWidgets  Qt${QT_VERSION_MAJOR}::WebSockets)
target_link_libraries(${PROJECT_NAME} Qt${QT_VERSION_MAJOR}::Network)



