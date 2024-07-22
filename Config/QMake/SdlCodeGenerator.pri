win32{
	QMAKE_RCC = rcc.exe
	COPY_FILE = copy
	SDL_GENERATOR_BIN = SdlCodeGenerator.exe
}
else{
	QMAKE_RCC = rcc
	COPY_FILE = cp
	SDL_GENERATOR_BIN = SdlCodeGenerator
}

isEmpty(SDL_GENERATOR_COMMAND_PARAM_MODIFICATORS) {
	SDL_GENERATOR_COMMAND_PARAM_MODIFICATORS = "--use-all-modificators"
}

SDL_GENERATOR_BIN = $$PWD/../../Bin/$$COMPILER_DIR/$$SDL_GENERATOR_BIN
SDL_GENERATOR_COMMAND_BASE = $$SDL_GENERATOR_BIN -S $$SDL_GENERATOR_SCHEME_PATH $$SDL_GENERATOR_COMMAND_PARAM_MODIFICATORS

# Paths-fix for windows
win32{
	SDL_GENERATOR_COMMAND_BASE ~= s,/,\\,g
	SDL_GENERATOR_SCHEME_PATH ~= s,/,\\,g
	SDL_GENERATOR_OUTPUT_DIR ~= s,/,\\,g
	SDL_GENERATOR_CPP_OUT_SUBFOLDER ~= s,/,\\,g
	SDL_GENERATOR_CPP_NAMESPACE ~= s,/,\\,g
	SDL_GENERATOR_QML_OUT_SUBFOLDER ~= s,/,\\,g
	SDL_GENERATOR_QML_MODULE_NAME ~= s,/,\\,g
}

message ("---------SDL Generator start")
message ("SDL_GENERATOR_SCHEME_PATH = '$$SDL_GENERATOR_SCHEME_PATH'")
message ("SDL_GENERATOR_CPP_OUT_SUBFOLDER = '$$SDL_GENERATOR_CPP_OUT_SUBFOLDER'")
message ("SDL_GENERATOR_CPP_NAMESPACE = '$$SDL_GENERATOR_CPP_NAMESPACE'")
message ("SDL_GENERATOR_QML_OUT_SUBFOLDER = '$$SDL_GENERATOR_QML_OUT_SUBFOLDER'")
message ("SDL_GENERATOR_QML_MODULE_NAME = '$$SDL_GENERATOR_QML_MODULE_NAME'")


#CPP

# ask sdl generator for output files
SDL_OUTPUT_FILES_CPP = $$system($$SDL_GENERATOR_COMMAND_BASE -D --CPP --GQL -N=$${SDL_GENERATOR_CPP_NAMESPACE} -O=$${SDL_GENERATOR_CPP_OUT_SUBFOLDER})

# custom build for CPP
cppSdlCompiler.name = SDL-Compiler-CPP
cppSdlCompiler.CONFIG += no_link
cppSdlCompiler.CONFIG += target_predeps
cppSdlCompiler.output = $$SDL_OUTPUT_FILES_CPP
cppSdlCompiler.commands = $$SDL_GENERATOR_COMMAND_BASE -G --CPP --GQL -N=$${SDL_GENERATOR_CPP_NAMESPACE} -O=$${SDL_GENERATOR_CPP_OUT_SUBFOLDER}
cppSdlCompiler.input = SDL_GENERATOR_SCHEME_PATH
cppSdlCompiler.variable_out = SOURCES
cppSdlCompiler.dependency_type = TYPE_C
cppSdlCompiler.depends = $$SDL_GENERATOR_SCHEME_PATH
QMAKE_EXTRA_COMPILERS += cppSdlCompiler


#QML

# ask sdl generator for output files
SDL_OUTPUT_FILES_QML = $$system($$SDL_GENERATOR_COMMAND_BASE -D --QML -N=$${SDL_GENERATOR_QML_MODULE_NAME} -O=$${SDL_GENERATOR_QML_OUT_SUBFOLDER})

# custom build for QML
qmlSdlCompiler.name = SDL-Compiler-QML
qmlSdlCompiler.CONFIG += no_link
qmlSdlCompiler.CONFIG += target_predeps
qmlSdlCompiler.output = $$SDL_OUTPUT_FILES_QML
qmlSdlCompiler.commands = $$SDL_GENERATOR_COMMAND_BASE -G --QML -N=$${SDL_GENERATOR_QML_MODULE_NAME} -O=$${SDL_GENERATOR_QML_OUT_SUBFOLDER}
qmlSdlCompiler.input = SDL_GENERATOR_SCHEME_PATH
qmlSdlCompiler.variable_out = SOURCES
qmlSdlCompiler.dependency_type = TYPE_C
qmlSdlCompiler.depends = $$SDL_GENERATOR_SCHEME_PATH
QMAKE_EXTRA_COMPILERS += qmlSdlCompiler
