win32{
	QMAKE_RCC = rcc.exe
	COPY_FILE = copy
	SDL_GENERATOR_BIN = DesignTokenCreator.exe
}
else{
	QMAKE_RCC = rcc
	COPY_FILE = cp
	SDL_GENERATOR_BIN = DesignTokenCreator
}

isEmpty(SDL_GENERATOR_COMMAND_PARAM_MODIFICATORS) {
	SDL_GENERATOR_COMMAND_PARAM_MODIFICATORS = "--use-all-modificators"
}

SDL_GENERATOR_BIN = $$PWD/../../Bin/$$COMPILER_DIR/$$SDL_GENERATOR_BIN
SDL_GENERATOR_COMMAND_BASE = $$SDL_GENERATOR_BIN -S $$SDL_GENERATOR_SCHEME_PATH -N $$SDL_GENERATOR_NAMESPACE -O $$SDL_GENERATOR_OUTPUT_DIR $$SDL_GENERATOR_COMMAND_PARAM_MODIFICATORS

# Paths-fix for windows
win32{
	SDL_GENERATOR_COMMAND_BASE ~= s,/,\\,g
	SDL_GENERATOR_SCHEME_PATH ~= s,/,\\,g
	SDL_GENERATOR_OUTPUT_DIR ~= s,/,\\,g
}

#CPP

# ask sdl generator for output files
SDL_OUTPUT_FILES_CPP = $$system($$SDL_GENERATOR_COMMAND_BASE -D --CPP --GQL)

# custom build for CPP
cppSdlCompiler.name = SDL-Compiler-CPP
cppSdlCompiler.CONFIG += no_link
cppSdlCompiler.CONFIG += target_predeps
cppSdlCompiler.output = $$SDL_OUTPUT_FILES_CPP
cppSdlCompiler.commands = $$SDL_GENERATOR_COMMAND_BASE -G --CPP --GQL
cppSdlCompiler.input = $$SDL_GENERATOR_SCHEME_PATH
cppSdlCompiler.variable_out = SOURCES
cppSdlCompiler.dependency_type = TYPE_C
cppSdlCompiler.depends = $$SDL_GENERATOR_SCHEME_PATH
QMAKE_EXTRA_COMPILERS += cppSdlCompiler


#QML

# ask sdl generator for output files
SDL_OUTPUT_FILES_QML = $$system($$SDL_GENERATOR_COMMAND_BASE -D --QML)

# custom build for QML
qmlSdlCompiler.name = SDL-Compiler-CPP
qmlSdlCompiler.CONFIG += no_link
qmlSdlCompiler.CONFIG += target_predeps
qmlSdlCompiler.output = $$SDL_OUTPUT_FILES_QML
qmlSdlCompiler.commands = $$SDL_GENERATOR_COMMAND_BASE -G --QML
qmlSdlCompiler.input = $$SDL_GENERATOR_SCHEME_PATH
qmlSdlCompiler.variable_out = SOURCES
qmlSdlCompiler.dependency_type = TYPE_C
qmlSdlCompiler.depends = $$SDL_GENERATOR_SCHEME_PATH
QMAKE_EXTRA_COMPILERS += qmlSdlCompiler
