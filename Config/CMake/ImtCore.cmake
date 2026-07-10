if(WIN32)
	target_link_libraries(${PROJECT_NAME} ${ACF_LIBRARY_LINK_SCOPE} advapi32 user32)
endif()

