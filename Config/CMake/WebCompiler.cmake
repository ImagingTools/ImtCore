
function(imtcore_compile_web)

	list(GET webdirs 1 WEB_BUILD_DIR)
	set(QRC_WEB_FILE ${WEB_BUILD_DIR}/Resources/${PROJECT_NAME}Web.qrc)
	set(QRC_CPP_WEB_FILE ${WEB_BUILD_DIR}/Resources/qrc_${PROJECT_NAME}Web.cpp)

	if(NOT PYTHONEXE)
		set(PYTHONEXE "$ENV{PYTHONEXE}")
		
		if(PYTHONEXE STREQUAL "")
			if (WIN32)
				set(PYTHONEXE python.exe)
			else()
				set(PYTHONEXE python3)
			endif()
		endif()
	endif()

	# Python path
	list(APPEND webdirs ${PYTHONEXE})

	add_custom_command(
		OUTPUT
		${CMAKE_CURRENT_BINARY_DIR}/__header.h # fake! ensure we run!
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

	if(NOT PYTHONEXE)
		set(PYTHONEXE "$ENV{PYTHONEXE}")
		
		if(PYTHONEXE STREQUAL "")
			if (WIN32)
				set(PYTHONEXE python.exe)
			else()
				set(PYTHONEXE python3)
			endif()
		endif()
	endif()

	message(VERBOSE "buildwebdir ${buildwebdir}")

	set(NODE_EXE node)

	if(${MSVC})
		set(NODE_EXE ${IMTCOREDIR}/3rdParty/nodejs/node.exe)
	endif()

	# Python path
	list(APPEND webdirs ${buildwebdir})

	#			COMMAND ${CMAKE_COMMAND} -E rm -rf ${buildwebdir}

	add_custom_command(
		OUTPUT
		${buildwebdir}/__header.h # fake! ensure we run!
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
		COMMAND ${CMAKE_COMMAND} -E copy ${buildwebdir}/src/jqml.full.js ${buildwebdir}/Resources/jqml.${resname}.js
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

	if(NOT PYTHONEXE)
		set(PYTHONEXE "$ENV{PYTHONEXE}")
		
		if(PYTHONEXE STREQUAL "")
			if (WIN32)
				set(PYTHONEXE python.exe)
			else()
				set(PYTHONEXE python3)
			endif()
		endif()
	endif()

	message(VERBOSE "buildwebdir ${buildwebdir}")

	if(NOT NODE_EXE)
		if(${MSVC})
			set(NODE_EXE ${IMTCOREDIR}/3rdParty/nodejs/node.exe)
		else()
			set(NODE_EXE node)
		endif()
	endif()

	list(LENGTH webdirs DIRS_COUNT)

	set(INDEX 0)
	set(DEPEND_LIST)

	message(VERBOSE "DIRS_COUNT ${DIRS_COUNT}")

	while(INDEX LESS DIRS_COUNT)
		list(GET webdirs ${INDEX} _FOLDER)
		file(GLOB_RECURSE FOUND_FILES "${_FOLDER}/*.qml")
		list(APPEND DEPEND_LIST ${FOUND_FILES})
		math(EXPR INDEX "${INDEX} + 2")
		set(INDEX ${INDEX})
	endwhile()

	message(VERBOSE "QRC_CPP_WEB_FILE ${QRC_CPP_WEB_FILE}")

	#			COMMAND ${CMAKE_COMMAND} -E rm -rf ${buildwebdir}

	add_custom_command(
			OUTPUT
		${buildwebdir}/Resources/jqml.${resname}.js
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory ${buildwebdir}
		COMMAND ${PYTHONEXE} ${IMTCOREDIR}/3rdParty/JQML2/preparesources.py ${webdirs}
		WORKING_DIRECTORY ${IMTCOREDIR}/3rdParty/JQML2
		COMMAND ${NODE_EXE} ${IMTCOREDIR}/3rdParty/JQML2/compiler/compiler.js ${buildwebdir}/src
		COMMAND ${CMAKE_COMMAND} -E copy ${buildwebdir}/src/jqml.full.js ${buildwebdir}/Resources/jqml.${resname}.js
		DEPENDS ${DEPEND_LIST} ${sdldependency}
		COMMENT "WEB COMPILER for ${PROJECT_NAME}"
		VERBATIM
		)

	add_custom_command(
		OUTPUT ${QRC_CPP_WEB_FILE}
		COMMAND
		Qt${QT_VERSION_MAJOR}::rcc
		ARGS
		-name ${PROJECT_NAME}Web ${QRC_WEB_FILE} -o ${QRC_CPP_WEB_FILE}
		DEPENDS
		${buildwebdir}/Resources/jqml.${resname}.js
		COMMENT
		"Compile QRC_WEB_FILE"
		)

	add_custom_target(
			WebCompiler${PROJECT_NAME} ALL
		DEPENDS ${QRC_CPP_WEB_FILE}
		)

	add_dependencies(WebCompiler${PROJECT_NAME} ${PROJECT_NAME})

	set(QRC_QRC_FILES)

	set_property(SOURCE ${QRC_CPP_WEB_FILE} PROPERTY SKIP_AUTOMOC ON)
	target_sources(${PROJECT_NAME} PRIVATE ${QRC_CPP_WEB_FILE})

endfunction(jqml_compile_web2)

function(jq_compile_web)
	# set(QRC_WEB_FILE ${buildwebdir}/Resources/${PROJECT_NAME}JsWeb.qrc)
	set(QRC_WEB_FILE ${buildwebdir}/Resources/qmlJsWeb.qrc)
	set(QRC_CPP_WEB_FILE ${buildwebdir}/Resources/qrc_${PROJECT_NAME}Web.cpp)

	if(NOT PYTHONEXE)
		set(PYTHONEXE "$ENV{PYTHONEXE}")

		if(PYTHONEXE STREQUAL "")
			if (WIN32)
				set(PYTHONEXE python.exe)
			else()
				set(PYTHONEXE python3)
			endif()
		endif()
	endif()

	message(VERBOSE "buildwebdir ${buildwebdir}")

	if(NOT NODE_EXE)
		if(${MSVC})
			set(NODE_EXE ${IMTCOREDIR}/3rdParty/nodejs/node.exe)
		else()
			set(NODE_EXE node)
		endif()
	endif()

	list(LENGTH webdirs DIRS_COUNT)

	set(INDEX 0)
	set(DEPEND_LIST)

	message(VERBOSE "DIRS_COUNT ${DIRS_COUNT}")

	while(INDEX LESS DIRS_COUNT)
		list(GET webdirs ${INDEX} _FOLDER)
		file(GLOB_RECURSE FOUND_FILES "${_FOLDER}/*.qml")
		list(APPEND DEPEND_LIST ${FOUND_FILES})
		math(EXPR INDEX "${INDEX} + 2")
		set(INDEX ${INDEX})
	endwhile()

	message(VERBOSE "QRC_CPP_WEB_FILE ${QRC_CPP_WEB_FILE}")
	list(APPEND webdirs_n ${IMTCOREDIR}/3rdParty/JQ/dist)
	list(APPEND webdirs_n ${buildwebdir}/Resources)

	message("PREPARE RESOURCES ${PYTHONEXE} ${IMTCOREDIR}/3rdParty/JQ/preparesources.py ${webdirs_n}")

	# HTML build
	add_custom_command(
		OUTPUT
		${buildwebdir}/Resources/index.html
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory ${buildwebdir}
		COMMAND ${PYTHONEXE} ${IMTCOREDIR}/3rdParty/JQ/preparesources.py ${webdirs_n}
		WORKING_DIRECTORY ${IMTCOREDIR}/3rdParty/JQ
		COMMAND ${CMAKE_COMMAND} -E env
		TARGETNAME=${TARGETNAME}
		${NODE_EXE}
		${IMTCOREDIR}/3rdParty/JQ/compiler/compiler.js
		-n index
		-i ${appicon}
		-o ${buildwebdir}/Resources/
		-m html
		DEPENDS ${DEPEND_LIST} ${sdldependency}
		COMMENT "Create html for ${PROJECT_NAME}"
		VERBATIM
	)

	# JS build
	add_custom_command(
		OUTPUT
		${buildwebdir}/Resources/index.js
		POST_BUILD
		WORKING_DIRECTORY ${IMTCOREDIR}/3rdParty/JQ
		COMMAND ${CMAKE_COMMAND} -E env
		TARGETNAME=${TARGETNAME}
		${NODE_EXE}
		${IMTCOREDIR}/3rdParty/JQ/compiler/compiler.js
		-c ${inputjs}
		-n index
		-o ${buildwebdir}/Resources/
		-r ${dataroot}
		-e ${startqml}
		-m js
		DEPENDS ${DEPEND_LIST} ${sdldependency}
		COMMENT "WEB COMPILER for ${PROJECT_NAME}"
		VERBATIM
	)

	add_custom_command(
		OUTPUT ${QRC_CPP_WEB_FILE}
		COMMAND
		Qt${QT_VERSION_MAJOR}::rcc
		ARGS
		-name ${PROJECT_NAME}Web
		${QRC_WEB_FILE}
		-o ${QRC_CPP_WEB_FILE}
		DEPENDS
		${buildwebdir}/Resources/index.js
		${buildwebdir}/Resources/index.html
		COMMENT
		"Compile QRC_WEB_FILE"
	)

	add_custom_target(
		WebCompiler${PROJECT_NAME} ALL
		DEPENDS ${QRC_CPP_WEB_FILE}
	)

	add_dependencies(WebCompiler${PROJECT_NAME} ${PROJECT_NAME})

	set(QRC_QRC_FILES)

	set_property(SOURCE ${QRC_CPP_WEB_FILE} PROPERTY SKIP_AUTOMOC ON)
	target_sources(${PROJECT_NAME} PRIVATE ${QRC_CPP_WEB_FILE})

endfunction(jq_compile_web)

macro(getImtBaseQmlWebDirs webdirs buildwebdir useImtControlsStyle)
	if (NOT DEFINED useImtControlsStyle)
		set(useImtControlsStyle TRUE)
	endif()

	list(APPEND ${webdirs} ${IMTCOREDIR}/3rdParty/JQML2/core/dist)
	list(APPEND ${webdirs} ${buildwebdir}/Resources)

	list(APPEND ${webdirs} ${IMTCOREDIR}/Include/imtstylecontrolsqml/Qml/Fonts)
	list(APPEND ${webdirs} ${buildwebdir}/Resources)

	if (useImtControlsStyle)
		list(APPEND ${webdirs} ${IMTCOREDIR}/Include/imtstylecontrolsqml/Qml/Acf)
		list(APPEND ${webdirs} ${buildwebdir}/src/Acf)
	endif()

	list(APPEND ${webdirs} ${IMTCOREDIR}/Qml/web)
	list(APPEND ${webdirs} ${buildwebdir}/Resources)

	list(APPEND ${webdirs} ${IMTCOREDIR}/Qml/imtgui)
	list(APPEND ${webdirs} ${buildwebdir}/src/imtgui)

	list(APPEND ${webdirs} ${IMTCOREDIR}/Qml/imtguigql)
	list(APPEND ${webdirs} ${buildwebdir}/src/imtguigql)

	list(APPEND ${webdirs} ${IMTCOREDIR}/Qml/imtcolgui)
	list(APPEND ${webdirs} ${buildwebdir}/src/imtcolgui)

	list(APPEND ${webdirs} ${IMTCOREDIR}/Qml/imtdocgui)
	list(APPEND ${webdirs} ${buildwebdir}/src/imtdocgui)

	list(APPEND ${webdirs} ${IMTCOREDIR}/Qml/imtcontrols)
	list(APPEND ${webdirs} ${buildwebdir}/src/imtcontrols)

	list(APPEND ${webdirs} ${IMTCOREDIR}/Impl/ImtCoreLoc/Translations)
	list(APPEND ${webdirs} ${buildwebdir}/Resources/Translations)

	list(APPEND ${webdirs} ${IMTCOREDIR_BUILD}/AuxInclude/${TARGETNAME}/GeneratedFiles/imtbasesdl/SDL/1.0/QML/imtbaseComplexCollectionFilterSdl)
	list(APPEND ${webdirs} ${buildwebdir}/src/imtbaseComplexCollectionFilterSdl)
	
	list(APPEND ${webdirs} ${IMTCOREDIR_BUILD}/AuxInclude/${TARGETNAME}/GeneratedFiles/imtbasesdl/SDL/1.0/QML/imtbaseDocumentCollectionFilterSdl)
	list(APPEND ${webdirs} ${buildwebdir}/src/imtbaseDocumentCollectionFilterSdl)

	list(APPEND ${webdirs} ${IMTCOREDIR_BUILD}/AuxInclude/${TARGETNAME}/GeneratedFiles/imtbasesdl/SDL/1.0/QML/imtbaseImtBaseTypesSdl)
	list(APPEND ${webdirs} ${buildwebdir}/src/imtbaseImtBaseTypesSdl)

	list(APPEND ${webdirs} ${IMTCOREDIR_BUILD}/AuxInclude/${TARGETNAME}/GeneratedFiles/imtbasesdl/SDL/1.0/QML/imtbaseImtCollectionSdl)
	list(APPEND ${webdirs} ${buildwebdir}/src/imtbaseImtCollectionSdl)

	list(APPEND ${webdirs} ${IMTCOREDIR_BUILD}/AuxInclude/${TARGETNAME}/GeneratedFiles/imtbasesdl/SDL/1.0/QML/imtbaseDocumentRevisionSdl)
	list(APPEND ${webdirs} ${buildwebdir}/src/imtbaseDocumentRevisionSdl)

	list(APPEND ${webdirs} ${IMTCOREDIR_BUILD}/AuxInclude/${TARGETNAME}/GeneratedFiles/imtappsdl/SDL/1.0/QML/imtappApplicationSdl)
	list(APPEND ${webdirs} ${buildwebdir}/src/imtappApplicationSdl)
	
	list(APPEND ${webdirs} ${IMTCOREDIR_BUILD}/AuxInclude/${TARGETNAME}/GeneratedFiles/imtbasesdl/SDL/1.0/QML/imtbaseSearchSdl)
	list(APPEND ${webdirs} ${buildwebdir}/src/imtbaseSearchSdl)
	
	list(APPEND ${webdirs} ${IMTCOREDIR_BUILD}/AuxInclude/${TARGETNAME}/GeneratedFiles/imtbasesdl/SDL/1.0/QML/imtbaseCommandsSdl)
	list(APPEND ${webdirs} ${buildwebdir}/src/imtbaseCommandsSdl)
	
	list(APPEND ${webdirs} ${IMTCOREDIR_BUILD}/AuxInclude/${TARGETNAME}/GeneratedFiles/imtbasesdl/SDL/1.0/QML/imtbaseSettingsSdl)
	list(APPEND ${webdirs} ${buildwebdir}/src/imtbaseSettingsSdl)

	list(APPEND ${webdirs} ${IMTCOREDIR_BUILD}/AuxInclude/${TARGETNAME}/GeneratedFiles/imtbasesdl/SDL/1.0/QML/imtbaseCollectionDocumentManagerSdl)
	list(APPEND ${webdirs} ${buildwebdir}/src/imtbaseCollectionDocumentManagerSdl)

	list(APPEND ${webdirs} ${IMTCOREDIR_BUILD}/AuxInclude/${TARGETNAME}/GeneratedFiles/imtbasesdl/SDL/1.0/QML/imtbaseCollectionImportSdl)
	list(APPEND ${webdirs} ${buildwebdir}/src/imtbaseCollectionImportSdl)

	list(APPEND ${webdirs} ${IMTCOREDIR_BUILD}/AuxInclude/${TARGETNAME}/GeneratedFiles/imtbasesdl/SDL/1.0/QML/imtbaseUndoManagerSdl)
	list(APPEND ${webdirs} ${buildwebdir}/src/imtbaseUndoManagerSdl)
	
	list(APPEND ${webdirs} ${IMTCOREDIR_BUILD}/AuxInclude/${TARGETNAME}/GeneratedFiles/imtbasesdl/SDL/1.0/QML/imtbaseDocumentIdFilterSdl)
	list(APPEND ${webdirs} ${buildwebdir}/src/imtbaseDocumentIdFilterSdl)

	list(APPEND ${webdirs} ${IMTCOREDIR_BUILD}/AuxInclude/${TARGETNAME}/GeneratedFiles/imtbasesdl/SDL/1.0/QML/imtbaseMimeTypeSdl)
	list(APPEND ${webdirs} ${buildwebdir}/src/imtbaseMimeTypeSdl)

	list(APPEND ${webdirs} ${IMTCOREDIR_BUILD}/AuxInclude/${TARGETNAME}/GeneratedFiles/imtbasesdl/SDL/1.0/QML/imtbaseProgressManagerSdl)
	list(APPEND ${webdirs} ${buildwebdir}/src/imtbaseProgressManagerSdl)

	list(APPEND ${webdirs} ${IMTCOREDIR_BUILD}/AuxInclude/${TARGETNAME}/GeneratedFiles/imt2dsdl/SDL/1.0/QML/imt2dGeometrySdl)
	list(APPEND ${webdirs} ${buildwebdir}/src/imt2dGeometrySdl)

	list(APPEND ${webdirs} ${IMTCOREDIR_BUILD}/AuxInclude/${TARGETNAME}/GeneratedFiles/imt2dsdl/SDL/1.0/QML/imt2dPaintSystemSdl)
	list(APPEND ${webdirs} ${buildwebdir}/src/imt2dPaintSystemSdl)

	list(APPEND ${webdirs} ${IMTCOREDIR_BUILD}/AuxInclude/${TARGETNAME}/GeneratedFiles/imtcolorsdl/SDL/1.0/QML/imtcolorRgbColorHexSdl)
	list(APPEND ${webdirs} ${buildwebdir}/src/imtcolorRgbColorHexSdl)

	list(APPEND ${webdirs} ${IMTCOREDIR_BUILD}/AuxInclude/${TARGETNAME}/GeneratedFiles/imtcolorsdl/SDL/1.0/QML/imtcolorLabSdl)
	list(APPEND ${webdirs} ${buildwebdir}/src/imtcolorLabSdl)
endmacro(getImtBaseQmlWebDirs)

macro(getImtLicQmlWebDirs webdirs buildwebdir)
	list(APPEND ${webdirs} ${IMTCOREDIR}/Qml/imtlicgui)
	list(APPEND ${webdirs} ${buildwebdir}/src/imtlicgui)

	list(APPEND ${webdirs} ${IMTCOREDIR_BUILD}/AuxInclude/${TARGETNAME}/GeneratedFiles/imtlicsdl/SDL/1.0/QML/imtlicFeaturesSdl)
	list(APPEND ${webdirs} ${buildwebdir}/src/imtlicFeaturesSdl)

	list(APPEND ${webdirs} ${IMTCOREDIR_BUILD}/AuxInclude/${TARGETNAME}/GeneratedFiles/imtlicsdl/SDL/1.0/QML/imtlicProductsSdl)
	list(APPEND ${webdirs} ${buildwebdir}/src/imtlicProductsSdl)

	list(APPEND ${webdirs} ${IMTCOREDIR_BUILD}/AuxInclude/${TARGETNAME}/GeneratedFiles/imtlicsdl/SDL/1.0/QML/imtlicLicensesSdl)
	list(APPEND ${webdirs} ${buildwebdir}/src/imtlicLicensesSdl)
endmacro(getImtLicQmlWebDirs)

macro(getImtAuthQmlWebDirs webdirs buildwebdir)
	list(APPEND ${webdirs} ${IMTCOREDIR}/Qml/imtauthgui)
	list(APPEND ${webdirs} ${buildwebdir}/src/imtauthgui)

	list(APPEND ${webdirs} ${IMTCOREDIR_BUILD}/AuxInclude/${TARGETNAME}/GeneratedFiles/imtauthsdl/SDL/1.0/QML/imtauthGroupsSdl)
	list(APPEND ${webdirs} ${buildwebdir}/src/imtauthGroupsSdl)

	list(APPEND ${webdirs} ${IMTCOREDIR_BUILD}/AuxInclude/${TARGETNAME}/GeneratedFiles/imtauthsdl/SDL/1.0/QML/imtauthRolesSdl)
	list(APPEND ${webdirs} ${buildwebdir}/src/imtauthRolesSdl)

	list(APPEND ${webdirs} ${IMTCOREDIR_BUILD}/AuxInclude/${TARGETNAME}/GeneratedFiles/imtauthsdl/SDL/1.0/QML/imtauthUsersSdl)
	list(APPEND ${webdirs} ${buildwebdir}/src/imtauthUsersSdl)

	list(APPEND ${webdirs} ${IMTCOREDIR_BUILD}/AuxInclude/${TARGETNAME}/GeneratedFiles/imtauthsdl/SDL/1.0/QML/imtauthAuthorizationSdl)
	list(APPEND ${webdirs} ${buildwebdir}/src/imtauthAuthorizationSdl)

	list(APPEND ${webdirs} ${IMTCOREDIR_BUILD}/AuxInclude/${TARGETNAME}/GeneratedFiles/imtauthsdl/SDL/1.0/QML/imtauthProfileSdl)
	list(APPEND ${webdirs} ${buildwebdir}/src/imtauthProfileSdl)

	list(APPEND ${webdirs} ${IMTCOREDIR_BUILD}/AuxInclude/${TARGETNAME}/GeneratedFiles/imtauthsdl/SDL/1.0/QML/imtauthSessionsSdl)
	list(APPEND ${webdirs} ${buildwebdir}/src/imtauthSessionsSdl)

	list(APPEND ${webdirs} ${IMTCOREDIR_BUILD}/AuxInclude/${TARGETNAME}/GeneratedFiles/imtauthsdl/SDL/1.0/QML/imtauthAuthorizationOptionsSdl)
	list(APPEND ${webdirs} ${buildwebdir}/src/imtauthAuthorizationOptionsSdl)

	list(APPEND ${webdirs} ${IMTCOREDIR_BUILD}/AuxInclude/${TARGETNAME}/GeneratedFiles/imtauthsdl/SDL/1.0/QML/imtauthPersonalAccessTokensSdl)
	list(APPEND ${webdirs} ${buildwebdir}/src/imtauthPersonalAccessTokensSdl)
endmacro(getImtAuthQmlWebDirs)

macro(getImtCoreQmlWebDirs webdirs buildwebdir)
	set(useImtControlsStyle TRUE)
	if(${ARGC} GREATER 2)
		set(useImtControlsStyle ${ARGV2})
	endif()

	getImtBaseQmlWebDirs(${webdirs} ${buildwebdir} ${useImtControlsStyle})
	getImtLicQmlWebDirs(${webdirs} ${buildwebdir})
	getImtAuthQmlWebDirs(${webdirs} ${buildwebdir})
endmacro(getImtCoreQmlWebDirs)
