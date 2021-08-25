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




# Paths-fix for windows
win32{
	DESIGN_TOKEN_CREATOR_EXE_BIN ~= s,/,\\,g
	DESIGN_TOKEN_CREATOR_COMMAND_PARAM_FILE ~= s,/,\\,g
	DESIGN_TOKEN_CREATOR_COMMAND_PARAM_INPUT_DIR ~= s,/,\\,g
	DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR ~= s,/,\\,g
}

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

DESIGN_TOKEN_CREATOR_COMMAND_PARAMS = -I -F $$DESIGN_TOKEN_CREATOR_COMMAND_PARAM_FILE -ID $$DESIGN_TOKEN_CREATOR_COMMAND_PARAM_INPUT_DIR -OD $$DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR -P $$TARGET
DESIGN_TOKEN_CREATOR_COMMAND = $$DESIGN_TOKEN_CREATOR_EXE_BIN $$DESIGN_TOKEN_CREATOR_COMMAND_PARAMS -env_vars $$ENV_VARS





GENERATE_RESOURCE_COMMANDS = $$DESIGN_TOKEN_CREATOR_COMMAND && $$[QT_INSTALL_BINS]/$$QMAKE_RCC -name ${QMAKE_FILE_BASE} $${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/${QMAKE_FILE_BASE}.qrc -o $${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/qrc_${QMAKE_FILE_BASE}.cpp
GENERATED_RESOURCE_OUTPUT = $${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/qrc_${QMAKE_FILE_BASE}.cpp

GENERATED_RESOURCES = $${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/imtguilight.qrc
GENERATED_RESOURCES += $${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/imtguidark.qrc

#GENERATED_RESOURCES = $$files($${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/imtguidark.qrc, false)
#GENERATED_RESOURCES += $$files($${DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR}/imtguilight.qrc, false)

win32{
	GENERATE_RESOURCE_COMMANDS ~= s,/,\\,g
	GENERATED_RESOURCE_OUTPUT ~= s,/,\\,g
}

generatedResources.name = Generated Resources Compiler
generatedResources.CONFIG += no_link target_predeps
generatedResources.output = $$GENERATED_RESOURCE_OUTPUT
generatedResources.commands = $$GENERATE_RESOURCE_COMMANDS
generatedResources.input = GENERATED_RESOURCES
generatedResources.variable_out = SOURCES
generatedResources.dependency_type = TYPE_C
generatedResources.depends += $$[QT_INSTALL_BINS]/$$QMAKE_RCC
QMAKE_EXTRA_COMPILERS += generatedResources


#generatedResources.name = Generated Resources Compiler
#generatedResources.CONFIG += no_link target_predeps
#generatedResources.output = $$GENERATED_RESOURCE_OUTPUT
#generatedResources.commands = $$DESIGN_TOKEN_CREATOR_COMMAND
#generatedResources.input = DESIGN_TOKEN_CREATOR_COMMAND_PARAM_FILE
#generatedResources.variable_out = RESOURCES
#generatedResources.dependency_type = TYPE_QRC
##generatedResources.depends += $$[QT_INSTALL_BINS]/$$QMAKE_RCC
#QMAKE_EXTRA_COMPILERS += generatedResources



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
