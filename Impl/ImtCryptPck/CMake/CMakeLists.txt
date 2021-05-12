project(ImtCryptPck LANGUAGES CXX)

include(${ACFDIR}/Config/CMake/ComponentConfig.cmake)
include(${ACFDIR}/Config/CMake/AcfStd.cmake)
include($ENV{IMTROOTDIR}/Config/CMake/OpenSSL.cmake)

target_link_libraries(${PROJECT_NAME} imtbase imtcrypt)

include_directories($ENV{IMTROOTDIR}/3rdParty/openssl/1.1)
