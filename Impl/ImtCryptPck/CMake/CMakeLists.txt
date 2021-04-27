project(ImtCryptPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${ACFDIR}/Config/CMake/AcfStd.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../../../../Config/CMake/OpenSSL.cmake)

target_link_libraries(${PROJECT_NAME} imtbase imtcrypt)

include_directories(${CMAKE_CURRENT_LIST_DIR}/../../../../3rdParty/openssl/1.1)
