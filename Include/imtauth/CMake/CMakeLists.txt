project(imtauth LANGUAGES CXX)	

include(${ACFDIR}/Config/CMake/StaticConfig.cmake)
include(${IMTCOREDIR}/Config/CMake/ImtCore.cmake)
include(${IMTCOREDIR}/Config/CMake/OpenSSL.cmake)

acf_use_qt_graphics_modules()

