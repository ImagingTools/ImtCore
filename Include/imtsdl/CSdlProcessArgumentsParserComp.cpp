#include <imtsdl/CSdlProcessArgumentsParserComp.h>


//Qt includes
#include <QtCore/QRegularExpression>
#include <QtCore/QCommandLineParser>
#include <QtCore/QCommandLineOption>

// ImtCore includes
#include <imtsdl/imtsdl.h>
#include <imtsdl/CSdlTools.h>


namespace imtsdl
{


// public methods

CSdlProcessArgumentsParserComp::CSdlProcessArgumentsParserComp()
:	m_isDependenciesMode(false),
	m_isGenerateMode(true),
	m_useAllModificators(false),
	m_notUseModificators(true),
	m_qmlEnabled(false),
	m_cppEnabled(true),
	m_generatorType(GT_CMAKE),
	m_autoJoinEnabled(false),
	m_autolinkLevel(ALL_NONE)
{
}


// reimplemented (ISdlProcessArgumentsParser)

bool CSdlProcessArgumentsParserComp::SetArguments(int argc, char** argv)
{
	QStringList arguments;
	for (int i = 0; i < argc; ++i){
		arguments << QString(argv[i]);
	}

	QCommandLineOption schemaFilePathOption({"S","schema-file"}, "SDL schema file path", "SchemaFilePath");
	QCommandLineOption outputDirectoryPathOption({"O","output-directory"}, "Directory where created files will be created", "OutputDirectoryPath");
	QCommandLineOption namespaceOption({"N","namespace"}, "Namespace, used in source and header files", "Namespace");
	QCommandLineOption namespacePrefixOption({"P","namespace-prefix"}, "Source and header files Namespace's prefix, will be added to the namespace in case, if namespace should be resolved from schema", "NamespacePrefix");
	QCommandLineOption generateOption({"G","generate"}, "Generate code from SDL mode");
	QCommandLineOption dependenciesOption({"D", "dependencies"}, "Generate a dependencies list to be generated. No code will be generated");
	QCommandLineOption modificatorsOption({"M", "modificator"}, "Modificator to generate code. You can provide multiple modificators. Note: modifier names are case sensitive.", "ModificatorList");
	QCommandLineOption allModificatorsOption("use-all-modificators", "Use all modificators to generate code");
	QCommandLineOption baseClassOption({"B", "base-class"}, "Defines base class of all generated classes with include path CLASS=/include/path", "BaseClassList");
	QCommandLineOption joinRulesOption({"J", "join"}, "Defines file types, will be joined TYPE(H or CPP)=/Destination/File/Path", "JoinRules");
	QCommandLineOption includePathOption({"I", "include"}, "Specifies the import directories which should be searched when parsing the schema.", "IncludePathList");
	QCommandLineOption autoJoinOption("auto-join", "Enables automatic join of output files into a single");
	QCommandLineOption generatorOption("generator", "{QMake | CMake}. Optional. Only for dependencies mode. Defines a type of output of files to be generated. Default - CMake", "generator");
	QCommandLineOption autoLinkOption("auto-link", "Defines the compilation order of the schema files.\n"
												   "0 - disabled. ALL files will be compiled.\n"
												   "1 - only those schemas with the same namespace as the original one will be compiled\n"
												   "2 - only the schema will be compiled. See the 'input parameter' option", "AutiLink", "0");
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
					namespacePrefixOption,
					generateOption,
					dependenciesOption,
					modificatorsOption,
					allModificatorsOption,
					qmlOption,
					cppOption,
					gqlOption,
					baseClassOption,
					joinRulesOption,
					includePathOption,
					autoJoinOption,
					generatorOption,
					autoLinkOption
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
	if (commandLineParser.isSet(namespacePrefixOption)){
		m_namespacePrefix = commandLineParser.value(namespacePrefixOption);
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
	if (commandLineParser.isSet(baseClassOption)){
		const QStringList baseClassList = commandLineParser.values(baseClassOption);
		for (const QString& baseClassDeclaration: baseClassList){
			if (!baseClassDeclaration.contains('=')){
				SendCriticalMessage(0, QString("Declaration '%1' does not contains include directive").arg(baseClassDeclaration));
				Q_ASSERT_X(false, "base class declarations parsing", "Declaration does not contains include directive");

				return false;
			}
			QString className = baseClassDeclaration.split('=')[0];
			QString includeDirective = baseClassDeclaration.split('=')[1];
			m_baseClassList.insert(className, includeDirective);
		}
	}

	if (commandLineParser.isSet(joinRulesOption)){
		const QStringList joinRules = commandLineParser.values(joinRulesOption);
		for (const QString& joinRule: joinRules){
			if (!joinRule.contains('=')){
				SendCriticalMessage(0, QString("Declaration '%1' does not contains destination path").arg(joinRule));
				Q_ASSERT_X(false, "destination path of join rule parsing", "Destination path is not set");

				return false;
			}

			QString fileType = joinRule.split('=')[0];
			if (fileType != s_headerFileType && fileType != s_sourceFileType){
				SendCriticalMessage(0, QString("Unknown file type. extected on of: %1 %2").arg(s_headerFileType, s_sourceFileType));
				I_CRITICAL();

				return false;
			}

			QString destinationPath = joinRule.split('=')[1];
			m_joinRules.insert(fileType, destinationPath);
		}
	}

	if (commandLineParser.isSet(includePathOption)){
		m_includePaths = commandLineParser.values(includePathOption);
	}

	m_autoJoinEnabled = commandLineParser.isSet(autoJoinOption);

	if (commandLineParser.isSet(autoLinkOption)){
		bool isDigit = false;

		const QString autoLinkLevelString = commandLineParser.value(autoLinkOption);
		const int autoLinkLevel =  autoLinkLevelString.toInt(&isDigit);
		if (!isDigit){
			SendErrorMessage(0, QString("Unexpected auto link [%1] argument value '%2'. See help for detales").arg(autoLinkOption.names().join('|'), autoLinkLevelString));

			return false;
		}

		const QList<int> acceptableValues = QList<int>({ALL_NONE, ALL_SAME_NAMESPACE, ALL_ONLY_FILE});
		if (!acceptableValues.contains(autoLinkLevel)){
			SendErrorMessage(0, QString("Unexpected auto link [%1] argument value '%2'. See help for detales").arg(autoLinkOption.names().join('|'), autoLinkLevelString));

			return false;
		}

		m_autolinkLevel = AutoLinkLevel(autoLinkLevel);
	}

	if (commandLineParser.isSet(generatorOption)){
		const QString generatorName = commandLineParser.value(generatorOption).toLower();
		if (generatorName == QStringLiteral("qmake")){
			m_generatorType = GT_QMAKE;
		}
		else if (generatorName == QStringLiteral("cmake")){
			m_generatorType = GT_CMAKE;
		}
		else {
			SendErrorMessage(0, QString("Unexpected generator option '%1'. Expected: QMake or CMake").arg(generatorName));
		}
	}

	// special modes
	bool isCppInParams = commandLineParser.isSet(cppOption);
	bool isQmlInParams = commandLineParser.isSet(qmlOption);
	bool isGqlInParams = commandLineParser.isSet(gqlOption);

	m_cppEnabled = isCppInParams || (!isQmlInParams && !isGqlInParams);
	m_qmlEnabled = isQmlInParams;
	m_gqlEnabled = isGqlInParams;

	m_notUseModificators = m_usedModificators.isEmpty();

	// only one mode must be used
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
	return m_namespace;
}


QString CSdlProcessArgumentsParserComp::GetNamespacePrefix() const
{
	return m_namespacePrefix;
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
	if (m_useAllModificators){
		return true;
	}

	if (m_notUseModificators){
		return false;
	}

	bool retVal = m_usedModificators.contains(modificatorName);

	// maybe modificators provided with option (i.e. contains : ) e.g. Json:Compact
	if (!retVal){
		static QRegularExpression regExp("^(.+)\\:");
		for (const QString& modificator: std::as_const(m_usedModificators)){
			QRegularExpressionMatch match = regExp.match(modificator);
			if (match.hasMatch()){
				const QString clearVal = match.capturedTexts().constLast();
				if (modificator == clearVal){
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


QMap<QString, QString> CSdlProcessArgumentsParserComp::GetBaseClassList() const
{
	return m_baseClassList;
}


QMap<QString, QString> CSdlProcessArgumentsParserComp::GetJoinRules() const
{
	return m_joinRules;
}


QStringList CSdlProcessArgumentsParserComp::GetIncludePaths() const
{
	return m_includePaths;
}


ISdlProcessArgumentsParser::GeneratorType CSdlProcessArgumentsParserComp::GetGeneratorType() const
{
	return m_generatorType;
}


bool CSdlProcessArgumentsParserComp::IsAutoJoinEnabled() const
{
	return m_autoJoinEnabled;
}


ISdlProcessArgumentsParser::AutoLinkLevel CSdlProcessArgumentsParserComp::GetAutoLinkLevel() const
{
	return m_autolinkLevel;
}


} // namespace imtsdl


