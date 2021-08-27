include($$(ACFDIR)/Config/QMake/GeneralConfig.pri)
#----------------------------------------------------------- The DESIGN TOKEN CREATOR config
# Setting the executable file name for specific OS
COMPILER_DIR = $$CONFIGURATION_NAME$$COMPILER_CODE
win32{
	DESIGN_TOKEN_CREATOR_EXE = DesignTokenCreatorExe.exe
}
else{
	DESIGN_TOKEN_CREATOR_EXE = DesignTokenCreatorExe
}
macx*{
	DESIGN_TOKEN_CREATOR_EXE = DesignTokenCreatorExe.app/Contents/MacOS/DesignTokenCreatorExe
}

#  Setting the executable absolutely file path for specific OS
DESIGN_TOKEN_CREATOR_EXE_BIN =$$PWD/../../Bin/$$COMPILER_DIR/$$DESIGN_TOKEN_CREATOR_EXE

win32{
	ARX_COMPILER = Arxc.exe
	ACF_TOOL = Acf.exe
	QMAKE_RCC = rcc.exe
	COPY_FILE = copy
}
else{
	ARX_COMPILER = Arxc
	ACF_TOOL = Acf
	QMAKE_RCC = rcc
	COPY_FILE = cp
}


DESIGN_TOKEN_CREATOR_COMMAND_PARAMS = -I -F ${QMAKE_FILE_IN} -ID $$DESIGN_TOKEN_CREATOR_COMMAND_PARAM_INPUT_DIR -OD $$DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR -P $$TARGET
DESIGN_TOKEN_CREATOR_COMMAND = $$DESIGN_TOKEN_CREATOR_EXE_BIN $$DESIGN_TOKEN_CREATOR_COMMAND_PARAMS -env_vars $$ENV_VARS


GENERATE_RESOURCE_COMMANDS = $$[QT_INSTALL_BINS]/$$QMAKE_RCC -name ${QMAKE_FILE_BASE} $${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/${QMAKE_FILE_BASE}.qrc -o $${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/qrc_${QMAKE_FILE_BASE}.cpp
GENERATED_RESOURCE_OUTPUT = $${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/qrc_${QMAKE_FILE_BASE}.cpp

# Paths-fix for windows
win32{
	DESIGN_TOKEN_CREATOR_EXE_BIN ~= s,/,\\,g
	DESIGN_TOKEN_CREATOR_COMMAND_PARAM_FILE ~= s,/,\\,g
	DESIGN_TOKEN_CREATOR_COMMAND_PARAM_INPUT_DIR ~= s,/,\\,g
	DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR ~= s,/,\\,g

	GENERATE_RESOURCE_COMMANDS ~= s,/,\\,g
	GENERATED_RESOURCE_OUTPUT ~= s,/,\\,g
	GENERATED_RESOURCES ~= s,/,\\,g
	DESIGN_TOKEN_CREATOR_INPUT_THEME_FILES ~= s,/,\\,g
}

generatedResources1.name = Generated Resources Compiler 1
generatedResources1.commands = $${DESIGN_TOKEN_CREATOR_COMMAND} && $${GENERATE_RESOURCE_COMMANDS}
generatedResources1.input = DESIGN_TOKEN_CREATOR_INPUT_THEME_FILES
generatedResources1.output = $${GENERATED_RESOURCE_OUTPUT}
generatedResources1.dependency_type = TYPE_C
generatedResources1.variable_out = SOURCES
generatedResources1.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += generatedResources1



#message(GENERATED_RESOURCES = $$GENERATED_RESOURCES)



#defineReplace(addToHeaders2) {
##    source = $$1
##    source_split = $$split(source, ".")
##    source_without_extension = $$first(source_split)
##    HEADERS += ${QMAKE_VAR_OBJECTS_DIR}$${source_without_extension}_1.h
##    return(${QMAKE_VAR_OBJECTS_DIR}$${source_without_extension}_1.cpp)

#RESOURCES += $$files($${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/*.qrc, false)
#    return($$files($${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/*.qrc, false))
#}

##TEST = somefile.h
##test_c.output_function = addToHeaders
##test_c.input = TEST
##test_c.commands = cp ${QMAKE_FILE_IN} ${QMAKE_VAR_OBJECTS_DIR}${QMAKE_FILE_BASE}_1.cpp ; cp ${QMAKE_FILE_IN} ${QMAKE_VAR_OBJECTS_DIR}${QMAKE_FILE_BASE}_1.h
##test_c.variable_out = SOURCES
##test_c.name = MyTestC
##QMAKE_EXTRA_COMPILERS += test_c



#generatedResources0.name = Generated Resources Compiler0
#generatedResources0.output_function = addToHeaders2
#generatedResources0.CONFIG += no_link target_predeps
##generatedResources0.output = $$GENERATED_RESOURCES
#generatedResources0.commands = $$DESIGN_TOKEN_CREATOR_COMMAND
#generatedResources0.input = DESIGN_TOKEN_CREATOR_COMMAND_PARAM_FILE
#generatedResources0.variable_out = RESOURCES
#generatedResources0.dependency_type = TYPE_QRC
##generatedResources.depends += $$[QT_INSTALL_BINS]/$$QMAKE_RCC
#QMAKE_EXTRA_COMPILERS += generatedResources0



#generatedResources0.name = Generated Resources Compiler0
#generatedResources0.CONFIG += no_link target_predeps
#generatedResources0.output = $$DESIGN_TOKEN_CREATOR_COMMAND_PARAM_FILE
#generatedResources0.commands = $$DESIGN_TOKEN_CREATOR_COMMAND
#generatedResources0.input = DESIGN_TOKEN_CREATOR_COMMAND_PARAM_FILE
##generatedResources0.variable_out = RESOURCES
#generatedResources0.dependency_type = TYPE_C
##generatedResources.depends += $$[QT_INSTALL_BINS]/$$QMAKE_RCC
##QMAKE_EXTRA_COMPILERS += generatedResources0
##QMAKE_EXTRA_TARGETS += generatedResources0
##PRE_TARGETDEPS += generatedResources0

#generatedResources.name = Generated Resources Compiler
#generatedResources.CONFIG += no_link target_predeps
#generatedResources.output = $$GENERATED_RESOURCE_OUTPUT
#generatedResources.commands = $$GENERATE_RESOURCE_COMMANDS
#generatedResources.input = GENERATED_RESOURCES
#generatedResources.variable_out = SOURCES
#generatedResources.dependency_type = TYPE_C
##generatedResources.depends += $$[QT_INSTALL_BINS]/$$QMAKE_RCC
#generatedResources.depends += $$GENERATED_RESOURCES
#QMAKE_EXTRA_COMPILERS += generatedResources0 generatedResources






#generatedResourcesimtguidark0.name = Generated Resources Compiler imtguidark0
#generatedResourcesimtguidark0.CONFIG += no_link target_predeps
#generatedResourcesimtguidark0.output = $${QRC_QRC_CURRENT_FILE}
#generatedResourcesimtguidark0.commands = $${DESIGN_TOKEN_CREATOR_EXE_BIN} $${DESIGN_TOKEN_CREATOR_COMMAND_PARAMS}
#generatedResourcesimtguidark0.input = DESIGN_TOKEN_CREATOR_COMMAND_PARAM_FILE
#generatedResourcesimtguidark0.dependency_type = TYPE_C

#generatedResourcesimtguidark1.name = Generated Resources Compiler imtguidark1
#generatedResourcesimtguidark1.CONFIG += no_link target_predeps
#generatedResourcesimtguidark1.output = $${QRC_CPP_CURRENT_FILE}
#generatedResourcesimtguidark1.commands = $$[QT_INSTALL_BINS]/$$QMAKE_RCC -name ${_file} ${QRC_QRC_CURRENT_FILE} -o ${QRC_CPP_CURRENT_FILE}
#generatedResourcesimtguidark1.input = DESIGN_TOKEN_CREATOR_COMMAND_PARAM_FILE
#generatedResourcesimtguidark1.dependency_type = TYPE_C
#generatedResourcesimtguidark1.variable_out = SOURCES
#QMAKE_EXTRA_COMPILERS += generatedResources$${_file}0  generatedResources$${_file}1


#QRC_QRC_CURRENT_FILE = $${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/imtguidark.qrc
#QRC_CPP_CURRENT_FILE = $${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/qrc_imtguidark.cpp

#generatedResourcesimtguidark0.name = Generated Resources Compiler imtguidark0
#generatedResourcesimtguidark0.CONFIG += no_link target_predeps
#generatedResourcesimtguidark0.output = $${QRC_QRC_CURRENT_FILE}
#generatedResourcesimtguidark0.commands = $${DESIGN_TOKEN_CREATOR_EXE_BIN} $${DESIGN_TOKEN_CREATOR_COMMAND_PARAMS}
#generatedResourcesimtguidark0.input = DESIGN_TOKEN_CREATOR_COMMAND_PARAM_FILE
#generatedResourcesimtguidark0.dependency_type = TYPE_C

#generatedResourcesimtguidark1.name = Generated Resources Compiler imtguidark1
#generatedResourcesimtguidark1.CONFIG += no_link target_predeps
#generatedResourcesimtguidark1.output = $${QRC_CPP_CURRENT_FILE}
#generatedResourcesimtguidark1.commands = $$[QT_INSTALL_BINS]/$$QMAKE_RCC -name imtguidark $${QRC_QRC_CURRENT_FILE} -o $${QRC_CPP_CURRENT_FILE}
##generatedResourcesimtguidark1.input = DESIGN_TOKEN_CREATOR_COMMAND_PARAM_FILE
#generatedResourcesimtguidark1.dependency_type = TYPE_C
#generatedResourcesimtguidark1.variable_out = SOURCES

#QMAKE_EXTRA_COMPILERS += generatedResourcesimtguidark0  generatedResourcesimtguidark1


#generatedResources0.name = Generated Resources Compiler 0
#generatedResources0.CONFIG += no_link target_predeps
#generatedResources0.output = $${QRC_QRC_CURRENT_FILE}
#generatedResources0.commands = $${DESIGN_TOKEN_CREATOR_EXE_BIN} $${DESIGN_TOKEN_CREATOR_COMMAND_PARAMS}
#generatedResources0.input = DESIGN_TOKEN_CREATOR_COMMAND_PARAM_FILE
#generatedResources0.dependency_type = TYPE_C
#QMAKE_EXTRA_COMPILERS += generatedResources0

#_file = imtguidark
#_file2 = imtguilight

#QRC_QRC_CURRENT_FILE = $${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/$${_file}.qrc
#QRC_CPP_CURRENT_FILE = $${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/qrc_$${_file}.cpp

#QRC_QRC_CURRENT_FILE2 = $${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/$${_file2}.qrc
#QRC_CPP_CURRENT_FILE2 = $${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/qrc_$${_file2}.cpp

#win32{
#	QRC_CPP_CURRENT_FILE ~= s,/,\\,g
#	QRC_QRC_CURRENT_FILE ~= s,/,\\,g
#	QRC_CPP_CURRENT_FILE2 ~= s,/,\\,g
#	QRC_QRC_CURRENT_FILE2 ~= s,/,\\,g
#}


#generatedResources$${_file}1.name = Generated Resources Compiler $${_file}1
#generatedResources$${_file}1.CONFIG += no_link target_predeps
#generatedResources$${_file}1.output = $${QRC_CPP_CURRENT_FILE} $${QRC_CPP_CURRENT_FILE2}
#generatedResources$${_file}1.commands = $${DESIGN_TOKEN_CREATOR_EXE_BIN} $${DESIGN_TOKEN_CREATOR_COMMAND_PARAMS} && $$[QT_INSTALL_BINS]/$$QMAKE_RCC -name $${_file} $${QRC_QRC_CURRENT_FILE} -o $${QRC_CPP_CURRENT_FILE} && $$[QT_INSTALL_BINS]/$$QMAKE_RCC -name $${_file2} $${QRC_QRC_CURRENT_FILE2} -o $${QRC_CPP_CURRENT_FILE2}
#generatedResources$${_file}1.input = DESIGN_TOKEN_CREATOR_COMMAND_PARAM_FILE
#generatedResources$${_file}1.dependency_type = TYPE_C
#generatedResources$${_file}1.variable_out = SOURCES
#QMAKE_EXTRA_COMPILERS += generatedResources$${_file}1

#generatedResources$${_file}2.name = Generated Resources Compiler $${_file}1
#generatedResources$${_file}2.CONFIG += no_link target_predeps
#generatedResources$${_file}2.output = $${QRC_CPP_CURRENT_FILE2}
#generatedResources$${_file}2.commands = $${DESIGN_TOKEN_CREATOR_EXE_BIN} $${DESIGN_TOKEN_CREATOR_COMMAND_PARAMS} && $$[QT_INSTALL_BINS]/$$QMAKE_RCC -name $${_file} $${QRC_QRC_CURRENT_FILE} -o $${QRC_CPP_CURRENT_FILE} && $$[QT_INSTALL_BINS]/$$QMAKE_RCC -name $${_file2} $${QRC_QRC_CURRENT_FILE2} -o $${QRC_CPP_CURRENT_FILE2}
#generatedResources$${_file}2.input = DESIGN_TOKEN_CREATOR_COMMAND_PARAM_FILE
#generatedResources$${_file}2.dependency_type = TYPE_C
#generatedResources$${_file}2.variable_out = SOURCES
#QMAKE_EXTRA_COMPILERS += generatedResources$${_file}2

#DESIGN_TOKEN_CREATOR_OUTPUT_FILE_NAMES = imtguidark imtguilight

#for(_file, DESIGN_TOKEN_CREATOR_OUTPUT_FILE_NAMES) {

#QRC_QRC_CURRENT_FILE = $${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/$${_file}.qrc
#QRC_CPP_CURRENT_FILE = $${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/qrc_$${_file}.cpp

#win32{
#	QRC_CPP_CURRENT_FILE ~= s,/,\\,g
#	QRC_QRC_CURRENT_FILE ~= s,/,\\,g
#}

#generatedResources$${_file}0.name = Generated Resources Compiler $${_file}0
#generatedResources$${_file}0.CONFIG += no_link target_predeps
#generatedResources$${_file}0.output = $${QRC_QRC_CURRENT_FILE}
#generatedResources$${_file}0.commands = $${DESIGN_TOKEN_CREATOR_EXE_BIN} $${DESIGN_TOKEN_CREATOR_COMMAND_PARAMS}
#generatedResources$${_file}0.input = DESIGN_TOKEN_CREATOR_COMMAND_PARAM_FILE
#generatedResources$${_file}0.dependency_type = TYPE_C

#generatedResources$${_file}1.name = Generated Resources Compiler $${_file}1
#generatedResources$${_file}1.CONFIG += no_link target_predeps
#generatedResources$${_file}1.output = $${QRC_CPP_CURRENT_FILE}
#generatedResources$${_file}1.commands = $$[QT_INSTALL_BINS]/$$QMAKE_RCC -name $${_file} $${QRC_QRC_CURRENT_FILE} -o $${QRC_CPP_CURRENT_FILE}
#generatedResources$${_file}1.input = DESIGN_TOKEN_CREATOR_COMMAND_PARAM_FILE
#generatedResources$${_file}1.dependency_type = TYPE_C
#generatedResources$${_file}1.variable_out = SOURCES

#QMAKE_EXTRA_COMPILERS += generatedResources$${_file}0 generatedResources$${_file}1

#}

#for(res0, GENERATED_RESOURCES) {
#message(additing compiler ${GENERATED_RESOURCES_COUNTER} for_ $${res0})

#generatedResources$${GENERATED_RESOURCES_COUNTER}.name = Generated Resources Compiler0
#generatedResources$${GENERATED_RESOURCES_COUNTER}.CONFIG += no_link target_predeps
#generatedResources$${GENERATED_RESOURCES_COUNTER}.output = $${res0}
#generatedResources$${GENERATED_RESOURCES_COUNTER}.commands = $$DESIGN_TOKEN_CREATOR_COMMAND
#generatedResources$${GENERATED_RESOURCES_COUNTER}.input = DESIGN_TOKEN_CREATOR_COMMAND_PARAM_FILE
#generatedResources$${GENERATED_RESOURCES_COUNTER}.variable_out = RESOURCES
#generatedResources$${GENERATED_RESOURCES_COUNTER}.dependency_type = TYPE_QRC
##generatedResources.depends += $$[QT_INSTALL_BINS]/$$QMAKE_RCC
#QMAKE_EXTRA_COMPILERS += generatedResources$${GENERATED_RESOURCES_COUNTER}

#}



#generatedResources.name = Generated Resources Compiler
#generatedResources.CONFIG += no_link target_predeps
#generatedResources.output = $$GENERATED_RESOURCE_OUTPUT
#generatedResources.commands = $$GENERATE_RESOURCE_COMMANDS
#generatedResources.input = GENERATED_RESOURCES
#generatedResources.variable_out = SOURCES
#generatedResources.dependency_type = TYPE_C
#generatedResources.depends += $$[QT_INSTALL_BINS]/$$QMAKE_RCC
#QMAKE_EXTRA_COMPILERS += generatedResources

#generatedResources.name = Generated Resources Compiler
#generatedResources.CONFIG += no_link target_predeps
#generatedResources.output = $${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/imtguilight.qrc.imtguilight $${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/imtguidark.qrc
#generatedResources.commands = $$DESIGN_TOKEN_CREATOR_COMMAND
#generatedResources.input = GENERATED_RESOURCES
#generatedResources.variable_out = RESOURCES
#generatedResources.dependency_type = TYPE_QRC
##generatedResources.depends += $$[QT_INSTALL_BINS]/$$QMAKE_RCC
#QMAKE_EXTRA_COMPILERS += generatedResources

#generatedResources2.name = Generated Resources Compiler2
#generatedResources2.CONFIG += no_link target_predeps
#generatedResources2.output = $${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/imtguilight.qrc
#generatedResources2.commands = $$DESIGN_TOKEN_CREATOR_COMMAND
#generatedResources2.input = DESIGN_TOKEN_CREATOR_COMMAND_PARAM_FILE
#generatedResources2.variable_out = RESOURCES
#generatedResources2.dependency_type = TYPE_QRC
##generatedResources.depends += $$[QT_INSTALL_BINS]/$$QMAKE_RCC
#QMAKE_EXTRA_COMPILERS += generatedResources2



#designToken.name = Desigen Resources Compiler
#designToken.CONFIG += no_link target_predeps
##designToken.output = $${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
#designToken.commands = $$DESIGN_TOKEN_CREATOR_COMMAND
#designToken.input = MYFILES
#designToken.variable_out = RESOURCES
#designToken.dependency_type = TYPE_QRC
##designToken.depends += $$DESIGN_TOKEN_CREATOR_EXE_BIN
#QMAKE_EXTRA_COMPILERS += designToken



#designToken.name = designTokenCreator
#designToken.CONFIG += no_link target_predeps
#designToken.commands = $$DESIGN_TOKEN_CREATOR_COMMAND
##designToken.output = $$DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR/*.qrc
##designToken.input = $$DESIGN_TOKEN_CREATOR_COMMAND_PARAM_FILE
##designToken.variable_out = RESOURCES
##generatedResources.dependency_type = TYPE_QRC
##generatedResources.depends += $$DESIGN_TOKEN_CREATOR_EXE_BIN
#QMAKE_EXTRA_COMPILERS += designToken
#QMAKE_EXTRA_TARGETS += designToken
#PRE_TARGETDEPS += designToken

#win32{
#	ARX_COMPILER = Arxc.exe
#	ACF_TOOL = Acf.exe
#	QMAKE_RCC = rcc.exe
#	COPY_FILE = copy
#}
#else{
#	ARX_COMPILER = Arxc
#	ACF_TOOL = Acf
#	QMAKE_RCC = rcc
#	COPY_FILE = cp
#}

##GENERATE_RESOURCE_COMMANDS = $$[QT_INSTALL_BINS]/$$QMAKE_RCC -name ${QMAKE_FILE_BASE} $$AUXINCLUDEPATH/GeneratedFiles/${QMAKE_FILE_BASE}.qrc -o $${ACF_TRANSLATIONS_OUTDIR}//qrc_${QMAKE_FILE_BASE}.cpp

#GENERATED_RESOURCE_INPUT_L = $$DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR/imtguilight.qrc
#GENERATED_RESOURCE_INPUT_D = $$DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR/imtguidark.qrc

#GENERATED_RESOURCE_OUTPUT_L = $$DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR/qrc_imtguilight.cpp
#GENERATED_RESOURCE_OUTPUT_D = $$DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR/qrc_imtguidark.cpp

#GENERATE_RESOURCE_COMMANDS_L = $$[QT_INSTALL_BINS]/$$QMAKE_RCC -name imtguilight $$GENERATED_RESOURCE_INPUT_L -o $$GENERATED_RESOURCE_OUTPUT_L
#GENERATE_RESOURCE_COMMANDS_D = $$[QT_INSTALL_BINS]/$$QMAKE_RCC -name imtguidark $$GENERATED_RESOURCE_INPUT_D -o $$GENERATED_RESOURCE_OUTPUT_D



##GENERATED_RESOURCE_OUTPUT = $${ACF_TRANSLATIONS_OUTDIR}/qrc_${QMAKE_FILE_BASE}.cpp
#win32{

#	GENERATED_RESOURCE_INPUT_L ~= s,/,\\,g
#	GENERATED_RESOURCE_INPUT_D ~= s,/,\\,g

#	GENERATED_RESOURCE_OUTPUT_L ~= s,/,\\,g
#	GENERATED_RESOURCE_OUTPUT_D ~= s,/,\\,g

#	GENERATE_RESOURCE_COMMANDS_L ~= s,/,\\,g
#	GENERATE_RESOURCE_COMMANDS_D ~= s,/,\\,g

#	GENERATE_RESOURCE_COMMANDS ~= s,/,\\,g
#	GENERATED_RESOURCE_OUTPUT ~= s,/,\\,g
#}

##generatedResources.name = Generated Resources Compiler
##generatedResources.CONFIG += no_link target_predeps
##generatedResources.output = $$GENERATED_RESOURCE_OUTPUT
##generatedResources.commands = $$GENERATE_RESOURCE_COMMANDS
##generatedResources.input = GENERATED_RESOURCES
##generatedResources.variable_out = SOURCES
##generatedResources.dependency_type = TYPE_C
##generatedResources.depends += $$[QT_INSTALL_BINS]/$$QMAKE_RCC
##QMAKE_EXTRA_COMPILERS += generatedResources

#generatedResourcesL.name = Generated Resources CompilerL
#generatedResourcesL.CONFIG += no_link target_predeps
#generatedResourcesL.output = $$GENERATED_RESOURCE_OUTPUT_L
#generatedResourcesL.commands = $$GENERATE_RESOURCE_COMMANDS_L
#generatedResourcesL.input = $$GENERATED_RESOURCE_INPUT_L
#generatedResourcesL.variable_out = SOURCES
#generatedResourcesL.dependency_type = TYPE_C
##generatedResourcesL.depends += $$GENERATED_RESOURCE_INPUT_L
#QMAKE_EXTRA_COMPILERS += generatedResourcesL


#generatedResourcesD.name = Generated Resources CompilerD
#generatedResourcesD.CONFIG += no_link target_predeps
#generatedResourcesD.output = $$GENERATED_RESOURCE_OUTPUT_D
#generatedResourcesD.commands = $$GENERATE_RESOURCE_COMMANDS_D
#generatedResourcesD.input = $$GENERATED_RESOURCE_INPUT_D
#generatedResourcesD.variable_out = SOURCES
#generatedResourcesD.dependency_type = TYPE_C
#generatedResourcesD.depends += $$GENERATED_RESOURCE_INPUT_D
#QMAKE_EXTRA_COMPILERS += generatedResourcesD
