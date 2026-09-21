include_directories(${IMTCOREDIR}/3rdParty/libduckdb/include)

if(APPLE)
	target_link_libraries(${PROJECT_NAME} PUBLIC ${IMTCOREDIR}/3rdParty/libduckdb/lib/ClangOSX/libduckdb.dylib)
elseif(WIN32)
	target_link_libraries(${PROJECT_NAME} PUBLIC ${IMTCOREDIR}/3rdParty/libduckdb/lib/VC16_x64/duckdb.dll)
elseif(UNIX)
	message(WARNING "DuckDb.cmake: no prebuilt DuckDB binaries available for this platform in 3rdParty/libduckdb/lib")
endif()
