#include <imtsdl/CSdlProcessArgumentsParserComp.h>


//Qt includes
#include <QtCore/QCommandLineParser>
#include <QtCore/QCommandLineOption>

// imtsdl includes
#include <imtsdl/imtsdl.h>


namespace imtsdl
{


// public methods

CSdlProcessArgumentsParserComp::CSdlProcessArgumentsParserComp()
:	m_isDependenciesMode(false),
	m_isGenerateMode(true)
{
}


bool CSdlProcessArgumentsParserComp::SetArguments(int argc, char** argv)
{
	QStringList arguments;
	for (int i = 0; i < argc; ++i){
		arguments << QString(argv[i]);
	}

	QCommandLineOption schemaFilePathOption({"S","schema-file",}, "SDL schema file path", "SchemaFilePath");
	QCommandLineOption outputDirectoryPathOption({"O","output-directory",}, "Directory where created files will be created", "OutputDirectoryPath");
	QCommandLineOption namespaceOption({"N","namespace",}, "Namespace, used in source and header files", "Namespace");
	QCommandLineOption generateOption({"G","generate",}, "Generate code from SDL mode");
	QCommandLineOption dependenciesOption({"D","dependencies",}, "Generate a dependencies list to be generated. No code will be generated");

	QCommandLineParser commandLineParser;
	bool isOptionsAdded = commandLineParser.addOptions(
				{
					schemaFilePathOption,
					outputDirectoryPathOption,
					namespaceOption,
					generateOption,
					dependenciesOption
				});
	if (!isOptionsAdded){
		Q_ASSERT(false);

		return false;
	}

	commandLineParser.process(arguments);
	if (commandLineParser.isSet(schemaFilePathOption)){
		m_schemaFilePath = commandLineParser.value(schemaFilePathOption);
	}
	if (commandLineParser.isSet(outputDirectoryPathOption)){
		m_outputDirectoryPath = commandLineParser.value(outputDirectoryPathOption);
	}
	if (commandLineParser.isSet(namespaceOption)){
		m_namespace = commandLineParser.value(namespaceOption);
	}
	if (commandLineParser.isSet(generateOption)){
		m_isGenerateMode = true;
		m_isDependenciesMode = false;
	}
	if (commandLineParser.isSet(dependenciesOption)){
		m_isGenerateMode = false;
		m_isDependenciesMode = true;
	}

	Q_ASSERT(commandLineParser.isSet(generateOption) ^ commandLineParser.isSet(dependenciesOption));

	return true;
}


QString CSdlProcessArgumentsParserComp::GetSchemaFilePath() const
{
	Q_ASSERT(!m_schemaFilePath.isNull());

	return m_schemaFilePath;
}


QString CSdlProcessArgumentsParserComp::GetOutputDirectoryPath() const
{
	Q_ASSERT(!m_outputDirectoryPath.isNull());

	return m_outputDirectoryPath;
}


QString CSdlProcessArgumentsParserComp::GetNamespace() const
{
	Q_ASSERT(!m_namespace.isNull());

	return m_namespace;
}


bool CSdlProcessArgumentsParserComp::IsGenerateMode() const
{
	return m_isGenerateMode;
}


bool CSdlProcessArgumentsParserComp::IsDependenciesMode() const
{
	return m_isDependenciesMode;
}


} // namespace imtsdl


