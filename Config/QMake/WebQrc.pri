
GENERATE_RESOURCE_COMMANDS = $$WEB_COMMAND
GENERATED_RESOURCE_OUTPUT = $${buildwebdir}/Resources/qrc_$${TARGET}Web.cpp
win32{
    GENERATED_RESOURCE_OUTPUT ~= s,/,\\,g
}

generatedWebResources.name = WebResources-Compiler
arcCompiler.target = $${ARXC_OUTDIR}/C${QMAKE_FILE_BASE}.h
generatedWebResources.CONFIG += no_link
generatedWebResources.CONFIG += target_predeps
generatedWebResources.output = $$GENERATED_RESOURCE_OUTPUT
generatedWebResources.commands = $$GENERATE_RESOURCE_COMMANDS
generatedWebResources.input = GENERATED_RESOURCES
generatedWebResources.variable_out = SOURCES
generatedWebResources.dependency_type = TYPE_C
QMAKE_EXTRA_COMPILERS += generatedWebResources

