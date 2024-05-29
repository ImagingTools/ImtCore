#include <imtsdl/CSdlProcessArgumentsParserComp.h>


//Qt includes
#include <QtCore/QRegularExpression>
#include <QtCore/QCommandLineParser>
#include <QtCore/QCommandLineOption>

// imtsdl includes
#include <imtsdl/imtsdl.h>


namespace imtsdl
{


// public methods

CSdlProcessArgumentsParserComp::CSdlProcessArgumentsParserComp()
:	m_isDependenciesMode(false),
	m_isGenerateMode(true),
	m_useAllModificators(false),
	m_notUseModificators(true),
	m_qmlEnabled(false),
	m_cppEnabled(true)
{
}


bool CSdlProcessArgumentsParserComp::SetArguments(int argc, char** argv)
{
	QStringList arguments;
	for (int i = 0; i < argc; ++i){
		arguments << QString(argv[i]);
	}

	QCommandLineOption schemaFilePathOption({"S","schema-file"}, "SDL schema file path", "SchemaFilePath");
	QCommandLineOption outputDirectoryPathOption({"O","output-directory"}, "Directory where created files will be created", "OutputDirectoryPath");
	QCommandLineOption namespaceOption({"N","namespace"}, "Namespace, used in source and header files", "Namespace");
	QCommandLineOption generateOption({"G","generate"}, "Generate code from SDL mode");
	QCommandLineOption dependenciesOption({"D", "dependencies"}, "Generate a dependencies list to be generated. No code will be generated");
	QCommandLineOption modificatorsOption({"M", "modificator"}, "Modificator to generate code. You can provide multiple modificators. Note: modifier names are case sensitive.", "ModificatorList");
	QCommandLineOption allModificatorsOption("use-all-modificators", "Use all modificators to generate code");
	// special modes
	QCommandLineOption cppOption("CPP", "C++ Modificator to generate code. (enabled default)");
	QCommandLineOption qmlOption("QML", "QML Modificator to generate code. (disables CPP and GQL if it not setted explicit)");
	QCommandLineOption gqlOption("GQL", "GraphQL Modificator to generate GrqphQL wrap C++ code . (disables CPP and QML if it not setted explicit)");

	QCommandLineParser commandLineParser;
	bool isOptionsAdded = commandLineParser.addOptions(
				{
					schemaFilePathOption,
					outputDirectoryPathOption,
					namespaceOption,
					generateOption,
					dependenciesOption,
					modificatorsOption,
					allModificatorsOption,
					qmlOption,
					cppOption,
					gqlOption
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
	if (commandLineParser.isSet(modificatorsOption)){
		m_useAllModificators = false;
		m_notUseModificators = false;
		m_usedModificators = commandLineParser.values(modificatorsOption);
	}
	if (commandLineParser.isSet(allModificatorsOption)){
		m_useAllModificators = true;
		m_notUseModificators = false;
	}

	// special modes
	bool isCppInParams = commandLineParser.isSet(cppOption);
	bool isQmlInParams = commandLineParser.isSet(qmlOption);
	bool isGqlInParams = commandLineParser.isSet(gqlOption);

	m_cppEnabled = isCppInParams || (!isQmlInParams && !isGqlInParams);
	m_qmlEnabled = isQmlInParams;
	m_gqlEnabled = isGqlInParams;

	// only one mode must be used
	Q_ASSERT(commandLineParser.isSet(generateOption) ^ commandLineParser.isSet(dependenciesOption));

	Q_ASSERT(!(m_useAllModificators && m_notUseModificators));

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


bool CSdlProcessArgumentsParserComp::IsModificatorEnabled(const QString& modificatorName) const
{
	if (m_notUseModificators){
		return false;
	}
	if (m_useAllModificators){
		return true;
	}

	bool retVal = m_usedModificators.contains(modificatorName);

	// maybe modificators provided with option (i.e. contains : ) e.g. Json:Compact
	if (!retVal){
		static QRegularExpression regExp("^(.+)\\:");
		for (const QString& modificatorName: std::as_const(m_usedModificators)){
			QRegularExpressionMatch match = regExp.match(modificatorName);
			if (match.hasMatch()){
				const QString clearVal = match.capturedTexts().constLast();
				if (modificatorName == clearVal){
					return true;
				}
			}
		}
	}

	return retVal;
}


bool CSdlProcessArgumentsParserComp::IsQmlEnabled() const
{
	return m_qmlEnabled;
}


bool CSdlProcessArgumentsParserComp::IsCppEnabled() const
{
	return m_cppEnabled;
}

bool CSdlProcessArgumentsParserComp::IsGqlEnabled() const
{
	return m_gqlEnabled;
}


} // namespace imtsdl


