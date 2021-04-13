project(ImtLicenseGuiPck LANGUAGES CXX)

include($ENV{ACFCONFIGDIR}/CMake/ComponentConfig.cmake)
include($ENV{ACFSLNDIR}/Config/CMake/AcfSln.cmake)

target_link_libraries(${PROJECT_NAME} imeas iproc iauth iedge imtgui imtloggui imtbase imtwidgets imtlic imtlicgui imtrepogui)

include($ENV{ACFCONFIGDIR}/CMake/AcfQt.cmake)
include($ENV{ACFCONFIGDIR}/CMake/AcfStd.cmake)