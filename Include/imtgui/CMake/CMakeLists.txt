project(imtgui LANGUAGES CXX)

include($ENV{ACFCONFIGDIR}/CMake/StaticConfig.cmake)
include($ENV{ACFSLNDIR}/Config/CMake/AcfSln.cmake)

ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-D_UNICODE)

