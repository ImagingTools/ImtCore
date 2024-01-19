#include <imtsdl/CSdlProcessArgumentsParserComp.h>


//Qt includes
#include <QtCore/QCommandLineParser>
#include <QtCore/QCommandLineOption>

// imtsdl includes
#include <imtsdl/imtsdl.h>


namespace imtsdl
{

bool CSdlProcessArgumentsParserComp::SetArguments(int argc, char** argv)
{
	QStringList arguments;
	for (int i = 0; i < argc; ++i){
		arguments << QString(argv[i]);
	}

	QCommandLineOption schemaFilePathOption({"S","schema-file",}, "SDL schema file path", "SchemaFilePath");
	QCommandLineOption outputDirectoryPathOption({"O","output-directory",}, "Directory where created files will be created", "OutputDirectoryPath");

	QCommandLineParser commandLineParser;
	bool isOptionsAdded = commandLineParser.addOptions({schemaFilePathOption, outputDirectoryPathOption});
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


} // namespace imtsdl


