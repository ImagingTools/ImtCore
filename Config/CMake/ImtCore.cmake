include(${ACFDIR}/Config/CMake/AcfStd.cmake)
include(${IACFDIR}/Config/CMake/Iacf.cmake)

target_link_libraries(${PROJECT_NAME} advapi32 user32)


