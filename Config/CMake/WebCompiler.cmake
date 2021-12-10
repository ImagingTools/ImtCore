macro(get_target_name target_name)
	set(COMPILER_NAME "Clang")
    if(${MSVC})
        if(${MSVC_TOOLSET_VERSION} STREQUAL 100)
            set(COMPILER_NAME "VC10")
        endif()
        if(${MSVC_TOOLSET_VERSION} STREQUAL 110)
            set(COMPILER_NAME "VC11")
        endif()

        if(${MSVC_TOOLSET_VERSION} STREQUAL 120)
            set(COMPILER_NAME}"VC12")
        endif()

        if(${MSVC_TOOLSET_VERSION} STREQUAL 140)
            set(COMPILER_NAME "VC14")
        endif()
        if(${MSVC_TOOLSET_VERSION} STREQUAL 141)
            set(COMPILER_NAME "VC15")
        endif()
        if(${MSVC_TOOLSET_VERSION} STREQUAL 142)
            set(COMPILER_NAME "VC16")
        endif()

		if(${CMAKE_CL_64} STREQUAL 1)
			set(COMPILER_NAME "${COMPILER_NAME}_64")
		endif()

	elseif(${APPLE})

		if("${CMAKE_OSX_ARCHITECTURES}" STREQUAL "arm64")
			set(COMPILER_NAME "${COMPILER_NAME}OSX_arm64")
			add_compile_definitions(COMPILER_NAME=ClangOSX)
			add_compile_definitions(PLATFORM_CODE=arm64)
		else()
			set(COMPILER_NAME "${COMPILER_NAME}OSX_64")
			add_compile_definitions(COMPILER_NAME=ClangOSX)
			add_compile_definitions(PLATFORM_CODE=x64)
		endif()

	elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")

		if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
			set(COMPILER_NAME "GCCLinux_64")
		else()
			set(COMPILER_NAME "${COMPILER_NAME}_64")
		endif()
	endif()

	message("CMAKE_SYSTEM_NAME" "${CMAKE_SYSTEM_NAME}")
	message("CMAKE_CXX_COMPILER_ARCHITECTURE_ID " "${CMAKE_CL_64}")


	set(${target_name} "${COMPILER_NAME}")
endmacro()


function(imtcore_compile_web)

set(QRC_WEB_FILE ${IMTCOREDIR}/Bin/web/Resources/LisaHtml.qrc)
set(QRC_CPP_WEB_FILE ${IMTCOREDIR}/Bin/web/Resources/qrc_LisaHtml.cpp)

        add_custom_command(
			OUTPUT
			    ${CMAKE_CURRENT_BINARY_DIR}/__header.h  # fake! ensure we run!
				${QRC_WEB_FILE}
				PRE_BUILD
			COMMAND ${IMTCOREDIR}/3rdParty/Python/3.8/python.exe ${IMTCOREDIR}/3rdParty/WebCompiler/webcompiler.py ${webdirs}
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
			${Qt5Core_RCC_EXECUTABLE}
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
