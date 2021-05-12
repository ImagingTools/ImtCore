include_directories("${CMAKE_CURRENT_LIST_DIR}/../../3rdParty/pybind11/include")
include_directories("${CMAKE_CURRENT_LIST_DIR}/../../3rdParty/Python/3.8/include")
link_directories("${CMAKE_CURRENT_LIST_DIR}/../../../3rdParty/Python/3.8/libs")

target_link_libraries(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/../../3rdParty/Python/3.8/libs/python38.lib)

