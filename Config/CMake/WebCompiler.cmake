
function(imtcore_compile_web)

	list(GET webdirs 1 WEB_BUILD_DIR)
	set(QRC_WEB_FILE ${WEB_BUILD_DIR}/Resources/${PROJECT_NAME}Web.qrc)
	set(QRC_CPP_WEB_FILE ${WEB_BUILD_DIR}/Resources/qrc_${PROJECT_NAME}Web.cpp)
	set(PYTHONEXE ${IMTCOREDIR}/3rdParty/Python/3.8/python.exe)

	if (NOT WIN32)
		set(PYTHONEXE python3)
	endif()

	# Python path
	list(APPEND webdirs ${PYTHONEXE})

	add_custom_command(
		OUTPUT
		${CMAKE_CURRENT_BINARY_DIR}/__header.h  # fake! ensure we run!
		${QRC_WEB_FILE}
		PRE_BUILD
		COMMAND ${PYTHONEXE} ${IMTCOREDIR}/3rdParty/WebCompiler/webcompiler.py ${webdirs}
		COMMENT
		"WEB COMPILER for ${PROJECT_NAME}"
		)

	add_custom_target(
		CopyBuild${PROJECT_NAME} ALL

		DEPENDS
		${CMAKE_CURRENT_BINARY_DIR}/__header.h
		)

	add_custom_command(
		OUTPUT ${QRC_CPP_WEB_FILE}
		COMMAND
		Qt${QT_VERSION_MAJOR}::rcc
		ARGS
		-name ${PROJECT_NAME}Web ${QRC_WEB_FILE} -o ${QRC_CPP_WEB_FILE}
		DEPENDS
		${QRC_WEB_FILE}
		COMMENT
		"Compile QRC_WEB_FILE"
	    )

	add_custom_target(
		WebCompiler${PROJECT_NAME} ALL
		DEPENDS ${QRC_CPP_WEB_FILE}
	    )

	set(QRC_QRC_FILES)

	set_property(SOURCE ${QRC_CPP_WEB_FILE} PROPERTY SKIP_AUTOMOC ON)
	target_sources(${PROJECT_NAME} PRIVATE ${QRC_CPP_WEB_FILE})
endfunction(imtcore_compile_web)


function(jqml_compile_web)

	set(QRC_WEB_FILE ${buildwebdir}/Resources/${PROJECT_NAME}JsWeb.qrc)
	set(QRC_CPP_WEB_FILE ${buildwebdir}/Resources/qrc_${PROJECT_NAME}Web.cpp)

	set(PYTHONEXE ${IMTCOREDIR}/3rdParty/Python/3.8/python.exe)
	if (NOT WIN32)
		set(PYTHONEXE python3)
	endif()

	message("buildwebdir ${buildwebdir}")

	set(NODE_EXE node)

	if(${MSVC})
		set(NODE_EXE  ${IMTCOREDIR}/3rdParty/nodejs/node.exe)
	endif()

	# Python path
	list(APPEND webdirs ${buildwebdir})

	#               COMMAND ${CMAKE_COMMAND} -E rm -rf ${buildwebdir}

	add_custom_command(
		OUTPUT
		${buildwebdir}/__header.h  # fake! ensure we run!
		${QRC_WEB_FILE}
		PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory ${buildwebdir}
		COMMAND ${PYTHONEXE} ${IMTCOREDIR}/3rdParty/JQML/preparesources.py ${webdirs}
		WORKING_DIRECTORY ${IMTCOREDIR}/3rdParty/JQML
		COMMAND ${NODE_EXE} ${IMTCOREDIR}/3rdParty/JQML/Compiler/index.js ${buildwebdir}/src
		COMMENT "WEB COMPILER for ${PROJECT_NAME}"
	    )


	add_custom_target(
		CopyBuild${PROJECT_NAME} ALL

		DEPENDS
		${buildwebdir}/__header.h
	    )

	add_custom_command(
		OUTPUT ${QRC_CPP_WEB_FILE}
		COMMAND  ${CMAKE_COMMAND} -E copy ${buildwebdir}/src/jqml.full.js  ${buildwebdir}/Resources/jqml.${resname}.js
		COMMAND
		Qt${QT_VERSION_MAJOR}::rcc
		ARGS
		-name ${PROJECT_NAME}Web ${QRC_WEB_FILE} -o ${QRC_CPP_WEB_FILE}
		DEPENDS
		${QRC_WEB_FILE}
		COMMENT
		"Compile QRC_WEB_FILE"
	    )

	add_custom_target(
		WebCompiler${PROJECT_NAME} ALL
		DEPENDS ${QRC_CPP_WEB_FILE}
	    )

	set(QRC_QRC_FILES)

	set_property(SOURCE ${QRC_CPP_WEB_FILE} PROPERTY SKIP_AUTOMOC ON)
	target_sources(${PROJECT_NAME} PRIVATE ${QRC_CPP_WEB_FILE})

endfunction(jqml_compile_web)

function(jqml_compile_web2)

	set(QRC_WEB_FILE ${buildwebdir}/Resources/${PROJECT_NAME}JsWeb.qrc)
	set(QRC_CPP_WEB_FILE ${buildwebdir}/Resources/qrc_${PROJECT_NAME}Web.cpp)

	set(PYTHONEXE ${IMTCOREDIR}/3rdParty/Python/3.8/python.exe)
	if (NOT WIN32)
	        set(PYTHONEXE python3)
	endif()

	message("buildwebdir ${buildwebdir}")

	if(NOT NODE_EXE)
		if(${MSVC})
			set(NODE_EXE  ${IMTCOREDIR}/3rdParty/nodejs/node.exe)
		else()
			set(NODE_EXE node)
		endif()
	endif()

	list(LENGTH webdirs DIRS_COUNT)

	set(INDEX 0)
	set(DEPEND_LIST)

	message("DIRS_COUNT ${DIRS_COUNT}")

	while(INDEX LESS DIRS_COUNT)
		list(GET webdirs ${INDEX} _FOLDER)
		file(GLOB_RECURSE FOUND_FILES "${_FOLDER}/*.qml")
		# message("_FOLDER ${_FOLDER}/*.qml")
		list(APPEND DEPEND_LIST ${FOUND_FILES})
		math(EXPR INDEX "${INDEX} + 2")
		set(INDEX ${INDEX})
	endwhile()

	# message("DEPEND_LIST ${DEPEND_LIST}")

	#               COMMAND ${CMAKE_COMMAND} -E rm -rf ${buildwebdir}

	add_custom_command(
	        OUTPUT
		# ${buildwebdir}/__header.h  # fake! ensure we run!
		${QRC_WEB_FILE}
		${buildwebdir}/Resources/jqml.${resname}.js
		PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory ${buildwebdir}
		COMMAND ${PYTHONEXE} ${IMTCOREDIR}/3rdParty/JQML2/preparesources.py ${webdirs}
		WORKING_DIRECTORY ${IMTCOREDIR}/3rdParty/JQML2
		COMMAND ${NODE_EXE} ${IMTCOREDIR}/3rdParty/JQML2/compiler/compiler.js ${buildwebdir}/src
		COMMAND  ${CMAKE_COMMAND} -E copy ${buildwebdir}/src/jqml.full.js  ${buildwebdir}/Resources/jqml.${resname}.js
		DEPENDS ${DEPEND_LIST} ${sdldependency}
		COMMENT "WEB COMPILER for ${PROJECT_NAME}"
	    )

	add_custom_command(
		OUTPUT ${QRC_CPP_WEB_FILE}
		COMMAND
		Qt${QT_VERSION_MAJOR}::rcc
		ARGS
		-name ${PROJECT_NAME}Web ${QRC_WEB_FILE} -o ${QRC_CPP_WEB_FILE}
		DEPENDS
		${QRC_WEB_FILE} ${buildwebdir}/Resources/jqml.${resname}.js
		COMMENT
		"Compile QRC_WEB_FILE"
	    )

	add_custom_target(
	        WebCompiler${PROJECT_NAME} ALL
		DEPENDS ${QRC_CPP_WEB_FILE}
	    )

	set(QRC_QRC_FILES)

	set_property(SOURCE ${QRC_CPP_WEB_FILE} PROPERTY SKIP_AUTOMOC ON)
	target_sources(${PROJECT_NAME} PRIVATE ${QRC_CPP_WEB_FILE})

endfunction(jqml_compile_web2)
