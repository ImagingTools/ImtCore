include_directories("${CMAKE_CURRENT_LIST_DIR}/../../../3rdParty/pybind11/include")


include_directories("${CMAKE_CURRENT_LIST_DIR}/../../../3rdParty/Python/3.8/include")


#target_link_libraries(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/../../../../3rdParty/Python/3.8/libs/python38.lib)
target_link_libraries(${PROJECT_NAME} python38)




