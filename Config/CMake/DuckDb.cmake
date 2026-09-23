include_directories(${IMTCOREDIR}/3rdParty/libduckdb/include)


if(APPLE)
	target_link_libraries(${PROJECT_NAME} PUBLIC ${IMTCOREDIR}/3rdParty/libduckdb/lib/macOS_universal/libduckdb.dylib)
elseif(UNIX)
	target_link_libraries(${PROJECT_NAME} PUBLIC ${IMTCOREDIR}/3rdParty/libduckdb/lib/GCC_Linux_x64/libduckdb.so)
elseif(WIN32)
	target_compile_definitions(${PROJECT_NAME} PUBLIC DUCKDB_STATIC_BUILD)
	target_link_libraries(${PROJECT_NAME} PUBLIC ${IMTCOREDIR}/3rdParty/libduckdb/lib/VC_x64/duckdb.lib)

	# duckdb.dll has no runtime output dir of its own, so only copy it next to actual binaries
	get_target_property(IMTCORE_DUCKDB_TARGET_TYPE ${PROJECT_NAME} TYPE)
	if(NOT IMTCORE_DUCKDB_TARGET_TYPE STREQUAL "STATIC_LIBRARY")
		add_custom_command(
			TARGET ${PROJECT_NAME}
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
				${IMTCOREDIR}/3rdParty/libduckdb/lib/VC_x64/duckdb.dll
				$<TARGET_FILE_DIR:${PROJECT_NAME}>
			COMMENT "Copying duckdb.dll runtime dependency for ${PROJECT_NAME}"
			VERBATIM
		)
	endif()
	unset(IMTCORE_DUCKDB_TARGET_TYPE)
endif()

