project(imtzip LANGUAGES CXX)


include(${ACFDIR}/Config/CMake/StaticConfig.cmake)

message(VERBOSE "ENV IMTCOREDIR $ENV{IMTCOREDIR}")

include(${IMTCOREDIR}/Config/CMake/Quazip.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)

