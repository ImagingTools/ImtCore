
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
}

DESIGN_TOKEN_CREATOR_COMMAND_PARAMS = -I -F $$DESIGN_TOKEN_CREATOR_COMMAND_PARAM_FILE -ID $$DESIGN_TOKEN_CREATOR_COMMAND_PARAM_INPUT_DIR -OD $$DESIGN_TOKEN_CREATOR_COMMAND_PARAM_OUTPUT_DIR
DESIGN_TOKEN_CREATOR_COMMAND = $$DESIGN_TOKEN_CREATOR_EXE_BIN $$DESIGN_TOKEN_CREATOR_COMMAND_PARAMS -env_vars $$ENV_VARS


QMAKE_PRE_LINK = $$DESIGN_TOKEN_CREATOR_COMMAND
