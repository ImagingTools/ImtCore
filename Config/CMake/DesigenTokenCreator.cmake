
include(${ACFDIR}/Config/CMake/GeneralConfig.cmake)

#----------------------------------------------------------- The DESIGN TOKEN CREATOR config
# Setting the executable file name for specific OS
set(COMPILER_DIR ${CMAKE_BUILD_TYPE}${TARGETNAME})
if (win32)
	set(DESIGN_TOKEN_CREATOR_EXE "DesignTokenCreatorExe.exe")
else()
	set(DESIGN_TOKEN_CREATOR_EXE "DesignTokenCreatorExe")
endif()

#if(APPLE)
#	set(DESIGN_TOKEN_CREATOR_EXE "DesignTokenCreatorExe.app/Contents/MacOS/DesignTokenCreatorExe")
#endif()

#  Setting the executable absolutely file path for specific OS
set(DESIGN_TOKEN_CREATOR_EXE_BIN "${CMAKE_CURRENT_SOURCE_DIR}/../../../Bin/${COMPILER_DIR}/${DESIGN_TOKEN_CREATOR_EXE}")



#file(GLOB QRC_FILES_0 "${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/*.qrc")

#file(GLOB QRC_FILES "${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/imtguilight.qrc")
#file(GLOB QRC_FILES "${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/imtguidark.qrc")

#list(APPEND QRC_FILES_XML "${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/imtguilight.qrc")
#list(APPEND QRC_FILES_XML "${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/imtguidark.qrc")

#list(APPEND QRC_FILES "${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/qrc_imtguilight.cpp")
#list(APPEND QRC_FILES "${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/qrc_imtguidark.cpp")


#set_property(SOURCE ${QRC_FILES} PROPERTY SKIP_AUTOMOC ON)

##acf_wrap_resources2(RESOURCES_FILES "${QRC_FILES}")

#target_sources(${PROJECT_NAME} PRIVATE ${QRC_FILES})

set(DESIGN_TOKEN_CREATOR_COMMAND_PARAMS
	-I
	-F
		${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_FILE}
	-ID
		${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_INPUT_DIR}
	-OD
		${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}
	-P
		${PROJECT_NAME}
	)






#set(SOURCES foo.cxx foo.hxx)
#add_library(Foo ${SOURCES})

#set(trofiles)
#foreach(_file ${SOURCES})
#  string(REPLACE "." "_" file_tro ${_file})
#  set(file_tro "${file_tro}_tro")
#  add_custom_command(
#    OUTPUT ${file_tro}
#    COMMAND perl ${CMAKE_CURRENT_SOURCE_DIR}/trans.pl ${CMAKE_CURRENT_SOURCE_DIR}/${_file} -o ${file_tro}
#    DEPENDS ${_file}
#  )
#  list(APPEND trofiles ${file_tro})
#endforeach()
#add_custom_command(
#  OUTPUT Foo.trx
#  COMMAND perl ${CMAKE_CURRENT_SOURCE_DIR}/combine.pl ${trofiles} -o Foo.trx
#  DEPENDS ${trofiles}
#)
#add_custom_target(do_trofiles DEPENDS Foo.trx)
#add_dependencies(Foo do_trofiles)

set(QRC_RAW_FILES imtguilight imtguidark)

foreach(_file ${QRC_RAW_FILES})

	set(file_tro ${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/qrc_${_file}.cpp)
	set(file_tro_QRC ${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/${_file}.qrc)
	set_property(SOURCE ${file_tro} PROPERTY SKIP_AUTOMOC ON)
	target_sources(${PROJECT_NAME} PRIVATE ${file_tro})

endforeach()

set(trofiles)

foreach(_file ${QRC_RAW_FILES})

	string(REPLACE "." "_" file_tro ${_file})
	set(file_tro ${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/qrc_${_file}.cpp)
	set(file_tro_QRC ${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/${_file}.qrc)


	add_custom_command(
		OUTPUT
			${file_tro_QRC}
		COMMAND
			${DESIGN_TOKEN_CREATOR_EXE_BIN}
		ARGS
			${DESIGN_TOKEN_CREATOR_COMMAND_PARAMS}
		COMMENT
			"Creating 	OUTPUT ${file_tro_QRC}"
		VERBATIM
		)

	add_custom_command(
		OUTPUT ${file_tro}
		COMMAND
			${Qt5Core_RCC_EXECUTABLE}
		ARGS
			-name ${_file} ${file_tro_QRC} -o ${file_tro}
		DEPENDS
			${file_tro_QRC}
		COMMENT
			"CREATING ${file_tro}"
	)

	list(APPEND trofiles ${file_tro})

endforeach()


add_custom_command(
	OUTPUT
		${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/${PROJECT_NAME}DesignToekn.stamp
	COMMAND
		${CMAKE_COMMAND} -E touch ${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/${PROJECT_NAME}DesignToekn.stamp
	COMMENT
		"Creating images for ${PROJECT_NAME}"
	DEPENDS
		${trofiles}
	VERBATIM
	)

add_custom_target(
	designToekn ALL
	DEPENDS ${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/${PROJECT_NAME}DesignToekn.stamp
	)




#foreach( file_i ${QRC_FILES_XML})
#	string(REPLACE "." ";" FILE_BASE_NAMES ${file_i})
#	list (GET FILE_BASE_NAMES 0 FILE_BASE_NAME)
#	add_custom_command(
#	TARGET
#		${PROJECT_NAME}
#	COMMAND
#		${DESIGN_TOKEN_CREATOR_EXE_BIN}
#	ARGS
#		${DESIGN_TOKEN_CREATOR_COMMAND_PARAMS}
##	COMMAND
##		${QT_RCC_EXECUTABLE}
##	ARGS
##		-name ${FILE_BASE_NAME} ${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/${FILE_BASE_NAME}.qrc -o ${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/qrc_${FILE_BASE_NAME}.cpp
#	COMMENT
#		"Creating images for ${file_i} with ${FILE_BASE_NAME} RCC_EXE = ${QT_RCC_EXECUTABLE}"
#	)
#endforeach()

#[[
add_custom_command(
	OUTPUT
		${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/${PROJECT_NAME}DesignToekn.stamp
	COMMAND
		${DESIGN_TOKEN_CREATOR_EXE_BIN}
	ARGS
		${DESIGN_TOKEN_CREATOR_COMMAND_PARAMS}
	COMMAND
		${CMAKE_COMMAND} -E touch ${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/${PROJECT_NAME}DesignToekn.stamp
	COMMENT
		"Creating images for ${PROJECT_NAME}"
	VERBATIM
	)

add_custom_command(
	OUTPUT
		${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/qrc_imtguilight.cpp
	DEPENDS
		${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/${PROJECT_NAME}DesignToekn.stamp
	COMMAND
		${Qt5Core_RCC_EXECUTABLE}
	ARGS
		-name imtguilight ${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/imtguilight.qrc -o ${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/qrc_imtguilight.cpp
	COMMENT
		"CREATING qrc_imtguilight"
	)

add_custom_command(
	OUTPUT
		${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/qrc_imtguidark.cpp
	DEPENDS
		${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/${PROJECT_NAME}DesignToekn.stamp
	COMMAND
		${Qt5Core_RCC_EXECUTABLE}
	ARGS
		-name imtguidark ${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/imtguidark.qrc -o ${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/qrc_imtguidark.cpp
	COMMENT
		"CREATING qrc_imtguidark"
	)
]]



#add_custom_command(
#	OUTPUT
#		${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/${PROJECT_NAME}DesignToekn2.stamp
#	DEPENDS
#		${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/${PROJECT_NAME}DesignToekn.stamp
#	COMMAND
#		${CMAKE_COMMAND} -E touch ${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/${PROJECT_NAME}DesignToekn2.stamp
#	COMMENT
#		"Creating images2 for ${PROJECT_NAME} added resourced ${QRC_FILES}      QT_RCC_EXECUTABLE = ${QT_RCC_EXECUTABLE}        Qt5Core_RCC_EXECUTABLE =  ${Qt5Core_RCC_EXECUTABLE}"
#	VERBATIM
#	)



#add_custom_target(
#	designToekn ALL
#	DEPENDS ${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/qrc_imtguilight.cpp ${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/qrc_imtguidark.cpp
#	)




