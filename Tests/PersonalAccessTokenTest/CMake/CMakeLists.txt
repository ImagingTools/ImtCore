project(PersonalAccessTokenTest LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/TestConfig.cmake)
include(${ACFDIR}/Config/CMake/AcfQt.cmake)

target_link_libraries(${PROJECT_NAME} imtauth imtbase Qt5::Test)
