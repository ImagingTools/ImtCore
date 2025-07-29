#include <imtsdlgen/CSdlProcessArgumentsParserComp.h>


//Qt includes
#include <QtCore/QRegularExpression>
#include <QtCore/QCommandLineParser>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

// ImtCore includes
#include <imtsdlgen/imtsdlgen.h>
#include <imtsdl/CSdlTools.h>
#include <imtsdlgen/CConfigReader.h>


namespace imtsdlgen
{

// static helpers
void MakePathAbsolute(QString& path)
{
	QFileInfo fileInfo(path);
	path = fileInfo.absoluteFilePath();
}


void MakePathAbsolute(QStringList& paths)
{
	for (QString& path: paths){
		MakePathAbsolute(path);
	}
}


template <class T>
void UpdateValueFromOptionalValue(const std::optional<T>& source, T& destination)
{
	if (source){
		destination = *source;
	}
}


template <class T> // optional overload
void UpdateValueFromOptionalValueO(const T& source, T& destination)
{
	if (source){
		destination = *source;
	}
}


// public methods


// reimplemented (imtsdl::ISdlProcessArgumentsParser)

bool CSdlProcessArgumentsParserComp::SetArguments(const QStringList& arguments)
{
	QCommandLineParser commandLineParser;
	commandLineParser.addHelpOption();
	commandLineParser.addVersionOption();

	/// \todo add 'Template/*' options, for CMDL parser

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
	QCommandLineOption generatorOption("generator", "{QMake | CMake | CMake-pipe | DEPFILE}. Optional. Only for dependencies mode. Defines a type of output of files to be generated. Default - CMake.\nNote: DEPFILE also supports define a dep file path i.e.: DEPFILE:<FILE_PATH>", "generator");
	QCommandLineOption autoLinkOption("auto-link", "Defines the compilation order of the schema files.\n"
												   "0 - disabled. ALL files will be compiled.\n"
												   "2 - only the schema will be compiled. See the 'input parameter' option", "AutoLink", "0");
	QCommandLineOption includeHeadersOption({"H","include-headers"}, "List of directories to search for generated header files", "HeadersIncludes");
	QCommandLineOption autoJoinOption("auto-join", "Enables automatic join of output files into a single.");
	QCommandLineOption moduleIncludePathsOption({"E", "extend", "add-module-path"}, "Specifies the file locations where the modules could be found.", "ExternalModulePaths");
	QCommandLineOption depFileParhOption("DEPFILE", "Depfile, used by CMake to collect dependencies. MUST be a valid file path if 'generator' option is 'DEPFILE'. If the file exsists it will be overwritten!", "DEPFILE");
	QCommandLineOption moduleOutFilePathOption("module-out-path", "Defines a path where a module file should ble created. NOTE: this option enables 'generate-module' option");
	QCommandLineOption configFilePathOption("config", "Defines a path to a file with initial configuration. NOTE: The parameters from the configuration file may be overridden by command line arguments. WARNING: path MUST be a valid path to existing file!", "config");

	// special modes
	QCommandLineOption cppOption("CPP", "C++ Modificator to generate code. (enabled default)");
	QCommandLineOption qmlOption("QML", "QML Modificator to generate code. (disables CPP and GQL if it not setted explicit)");
	QCommandLineOption gqlOption("GQL", "GraphQL Modificator to generate GrqphQL wrap C++ code . (disables CPP and QML if it not setted explicit)");
	QCommandLineOption schemaDependencyOption("schema-dependencies",
				"Schema dependency collection mode: This mode displays a list of all schemas that affect the generated code. "
						"Only the 'generator', 'include' and 'schema-file (S)' options SHOULD be used in conjunction with this mode. "
						"You MUST NOT use other options, in this case, the behavior is undefined!");

	QCommandLineOption moduleGenerationOption("generate-module", "Enables module generation");


	QList<QCommandLineOption> allOptions = PrepareCommandLineOptions();
	allOptions << QList<QCommandLineOption>({
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
				schemaDependencyOption,
				baseClassOption,
				joinRulesOption,
				includePathOption,
				autoJoinOption,
				generatorOption,
				autoLinkOption,
				includeHeadersOption,
				moduleIncludePathsOption,
				depFileParhOption,
				moduleOutFilePathOption,
				moduleGenerationOption,
				configFilePathOption
	});

	bool isOptionsAcceptable = true;

	QSet<QString> optionNames;
	QSet<QString> optionValueNames;
	for (const QCommandLineOption& option: allOptions){
		// check option names
		for (const QString& optionName: option.names()){
			if (optionNames.contains(optionName)){
				SendCriticalMessage(0, QString("Duplication of option '%1' is detected. All option IDs MUST be unique! Select another name for this option").arg(optionName));
				isOptionsAcceptable = false;
			}
			optionNames << optionName;
		}

		// check option value names. Empty names can be duplicated
		const QString optionValueName = option.valueName();
		if (!optionValueName.isEmpty()){
			if (optionValueNames.contains(optionValueName)){
				SendCriticalMessage(0, QString("Duplication of option value ID '%1' is detected. All option value's IDs MUST be unique! Select another name for this option").arg(optionValueName));
				isOptionsAcceptable = false;
			}
			optionValueNames << optionValueName;
		}
	}
	if (!isOptionsAcceptable){
		Q_ASSERT_X(false, "Unacceptable options!", __func__);

		return false;
	}


	bool isOptionsAdded = commandLineParser.addOptions(allOptions);

	if (!isOptionsAdded){
		I_CRITICAL();

		return false;
	}

	commandLineParser.process(arguments);

	// first read the config
	if (commandLineParser.isSet(configFilePathOption)){
		const bool isRead = ReadFromSettings(commandLineParser.value(configFilePathOption));
		if (!isRead){
			SendErrorMessage(0, "Unable to read config file. Aborting...");

			return false;
		}
	}

	// then override/add parameters, provided by command line
	if (commandLineParser.isSet(schemaFilePathOption)){
		m_schemaFilePath = commandLineParser.value(schemaFilePathOption);
		MakePathAbsolute(m_schemaFilePath);
	}
	if (commandLineParser.isSet(outputDirectoryPathOption)){
		m_outputDirectoryPath = commandLineParser.value(outputDirectoryPathOption);
		MakePathAbsolute(m_outputDirectoryPath);
	}
	if (commandLineParser.isSet(namespaceOption)){
		m_namespace = commandLineParser.value(namespaceOption);
	}
	if (commandLineParser.isSet(namespacePrefixOption)){
		m_namespacePrefix = commandLineParser.value(namespacePrefixOption);
	}
	if (!m_namespacePrefix.isNull()){
		imtsdl::CSdlTools::s_sdlGlobalPrefix = m_namespacePrefix;
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
			MakePathAbsolute(destinationPath);
			m_joinRules.insert(fileType, destinationPath);
		}
	}

	if (commandLineParser.isSet(includePathOption)){
		m_includePaths = commandLineParser.values(includePathOption);
		MakePathAbsolute(m_includePaths);
	}

	if (!m_autoJoinEnabled.has_value()){
		m_autoJoinEnabled = commandLineParser.isSet(autoJoinOption);
	}

	if (commandLineParser.isSet(autoLinkOption)){
		bool isDigit = false;

		const QString autoLinkLevelString = commandLineParser.value(autoLinkOption);
		const int autoLinkLevel =  autoLinkLevelString.toInt(&isDigit);
		if (!isDigit){
			SendErrorMessage(0, QString("Unexpected auto link [%1] argument value '%2'. See help for details").arg(autoLinkOption.names().join('|'), autoLinkLevelString));

			return false;
		}

		const QList<int> acceptableValues = QList<int>({ALL_NONE, ALL_ONLY_FILE});
		if (!acceptableValues.contains(autoLinkLevel)){
			SendErrorMessage(0, QString("Unexpected auto link [%1] argument value '%2'. See help for details").arg(autoLinkOption.names().join('|'), autoLinkLevelString));

			return false;
		}

		m_autoLinkLevel = AutoLinkLevel(autoLinkLevel);
	}

	if (commandLineParser.isSet(generatorOption)){
		const QString originalGeneratorName = commandLineParser.value(generatorOption);
		const QString generatorName = originalGeneratorName.toLower();
		if (generatorName == QStringLiteral("qmake")){
			m_generatorType = GT_QMAKE;
		}
		else if (generatorName == QStringLiteral("cmake")){
			m_generatorType = GT_CMAKE;
		}
		else if (generatorName == QStringLiteral("cmake-pipe")){
			m_generatorType = GT_CMAKE_PIPE;
		}
		else if (originalGeneratorName.startsWith(QStringLiteral("DEPFILE"))){
			m_generatorType = GT_DEP_FILE;
			// also set a path if it defined with 'generator option' DEPFILE:C:\depfile.txt
			if (originalGeneratorName.contains(':')){
				QStringList fileParts = originalGeneratorName.split(':');
				fileParts.removeFirst(); ///< remove 'DEPFILE' part
				if (!fileParts.isEmpty()){
					m_depFilePath = fileParts.join(':');
				}
			}
		}
		else {
			SendErrorMessage(0, QString("Unexpected generator option '%1'. See %2 help for details").arg(generatorName, generatorOption.names().join('/')));
		}
	}

	if (commandLineParser.isSet(includeHeadersOption)){
		m_headersIncludePaths = commandLineParser.values(includeHeadersOption);
		MakePathAbsolute(m_headersIncludePaths);
	}

	if (commandLineParser.isSet(moduleIncludePathsOption)){
		m_moduleIncludePathList = commandLineParser.values(moduleIncludePathsOption);
		MakePathAbsolute(m_moduleIncludePathList);
	}

	if (commandLineParser.isSet(depFileParhOption)){
		m_depFilePath = commandLineParser.value(depFileParhOption);
		MakePathAbsolute(m_depFilePath);
	}
	if (commandLineParser.isSet(moduleOutFilePathOption)){
		m_moduleOutputFilePath = commandLineParser.value(moduleOutFilePathOption);
		m_isModuleGenerationEnabled = true;
	}

	// special modes
	bool isCppInParamSet = commandLineParser.isSet(cppOption);
	bool isQmlInParamSet = commandLineParser.isSet(qmlOption);
	bool isGqlInParamSet = commandLineParser.isSet(gqlOption);

	if (!m_cppEnabled.has_value()){
		m_cppEnabled = isCppInParamSet || (!isQmlInParamSet && !isGqlInParamSet);
	}
	if (!m_qmlEnabled.has_value()){
		m_qmlEnabled = isQmlInParamSet;
	}
	if (!m_gqlEnabled.has_value()){
		m_gqlEnabled = isGqlInParamSet;
	}

	if (commandLineParser.isSet(moduleGenerationOption)){
		m_isModuleGenerationEnabled = true;
	}

	// schema dependency mode - is a special mode. Cleanup all unnecessary values
	if (commandLineParser.isSet(schemaDependencyOption)){
		m_schemaDependencyModeEnabled = true;
		m_isGenerateMode = false;
		m_cppEnabled = false;
		m_qmlEnabled = false;
		m_gqlEnabled = false;
		m_isDependenciesMode = false;
		m_usedModificators.clear();
	}

	m_notUseModificators = m_usedModificators.isEmpty();

	// only one mode MUST be used
	if (!(commandLineParser.isSet(generateOption) ^ commandLineParser.isSet(dependenciesOption) ^ commandLineParser.isSet(schemaDependencyOption))){
		SendErrorMessage(0, "Multiple uncompatible modes activated. Select only one of: -G -D --schema-dependencies");

		return false;
	}

	if (m_generatorType == GT_DEP_FILE){
		QFileInfo depFileInfo(m_depFilePath);
		if (depFileInfo.exists()){
			const bool removed = QFile::remove(depFileInfo.absoluteFilePath());
			if (!removed){
				SendErrorMessage(0, QString("Unable to remove dependency file '%1'").arg(depFileInfo.absoluteFilePath()));

				return false;
			}
		}

		QDir depFileDir = depFileInfo.dir();
		if (!depFileDir.exists()){
			const bool dirCreated = depFileDir.mkpath(depFileDir.absolutePath());
			if (!dirCreated){
				SendErrorMessage(0, QString("Unable to create folder '%1'").arg(depFileDir.absolutePath()));

				return false;
			}
		}
	}

	bool retVal = ProcessCommandLineOptions(commandLineParser);

	return retVal;
}


QString CSdlProcessArgumentsParserComp::GetSchemaFilePath() const
{
	return m_schemaFilePath;
}


QString CSdlProcessArgumentsParserComp::GetOutputDirectoryPath() const
{
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
	return m_isGenerateMode.value_or(true);
}


bool CSdlProcessArgumentsParserComp::IsDependenciesMode() const
{
	return m_isDependenciesMode.value_or(false);
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
	return m_qmlEnabled.value_or(false);
}


bool CSdlProcessArgumentsParserComp::IsCppEnabled() const
{
	return m_cppEnabled.value_or(false);
}


bool CSdlProcessArgumentsParserComp::IsGqlEnabled() const
{
	return m_gqlEnabled.value_or(false);
}


bool CSdlProcessArgumentsParserComp::IsSchemaDependencyModeEnabled() const
{
	return m_schemaDependencyModeEnabled.value_or(false);
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


imtsdl::ISdlProcessArgumentsParser::GeneratorType CSdlProcessArgumentsParserComp::GetGeneratorType() const
{
	return *m_generatorType;
}


bool CSdlProcessArgumentsParserComp::IsAutoJoinEnabled() const
{
	return m_autoJoinEnabled.value_or(false);
}


imtsdl::ISdlProcessArgumentsParser::AutoLinkLevel CSdlProcessArgumentsParserComp::GetAutoLinkLevel() const
{
	return m_autoLinkLevel.value_or(ALL_NONE);
}


QStringList CSdlProcessArgumentsParserComp::GetHeadersIncludePaths() const
{
	return m_headersIncludePaths;
}


QStringList CSdlProcessArgumentsParserComp::GetModuleIncludePaths() const
{
	return m_moduleIncludePathList;
}


QString CSdlProcessArgumentsParserComp::GetDepFilePath() const
{
	return m_depFilePath;
}


QString CSdlProcessArgumentsParserComp::GetModuleOutputFilePath() const
{
	return m_moduleOutputFilePath;
}


bool CSdlProcessArgumentsParserComp::IsModuleGenerateEnabled() const
{
	return m_isModuleGenerationEnabled.value_or(false);
}


bool CSdlProcessArgumentsParserComp::IsTemplateEnabled() const
{
	return m_isTemplateEnabled.value_or(false);
}


QString CSdlProcessArgumentsParserComp::GetTemplateIncludePath() const
{
	return m_includePathTemplate;
}


QString CSdlProcessArgumentsParserComp::GetTemplateOutputPath() const
{
	return m_outputDirTemplate;
}


QString CSdlProcessArgumentsParserComp::GetTemplateQmlOutputPath() const
{
	return m_outputQmlDirTemplate;
}


bool CSdlProcessArgumentsParserComp::ReadFromSettings(const QString& settingsFilePath)
{

	CConfigReader configReader;
	const bool isRead = configReader.ReadFromFile(settingsFilePath);
	if(!isRead){
		return false;
	}

	UpdateValueFromOptionalValue(configReader.schemaFilePath, m_schemaFilePath);
	UpdateValueFromOptionalValue(configReader.outputDirectoryPath, m_outputDirectoryPath);
	UpdateValueFromOptionalValue(configReader.namespaceValue, m_namespace);
	UpdateValueFromOptionalValue(configReader.namespacePrefix, m_namespacePrefix);
	UpdateValueFromOptionalValueO(configReader.dependenciesMode, m_isDependenciesMode);
	UpdateValueFromOptionalValueO(configReader.generateMode, m_isGenerateMode);
	UpdateValueFromOptionalValue(configReader.enabledModificators, m_usedModificators);
	UpdateValueFromOptionalValueO(configReader.useAllModificators, m_useAllModificators);
	UpdateValueFromOptionalValueO(configReader.cppEnabled, m_cppEnabled);
	UpdateValueFromOptionalValueO(configReader.qmlEnabled, m_qmlEnabled);
	UpdateValueFromOptionalValueO(configReader.gqlEnabled, m_gqlEnabled);
	UpdateValueFromOptionalValueO(configReader.schemaDependencyModeEnabled, m_schemaDependencyModeEnabled);
	UpdateValueFromOptionalValue(configReader.baseClassList, m_baseClassList);
	UpdateValueFromOptionalValue(configReader.includePaths, m_includePaths);
	UpdateValueFromOptionalValueO(configReader.generatorType, m_generatorType);
	UpdateValueFromOptionalValueO(configReader.autoJoinEnabled, m_autoJoinEnabled);
	UpdateValueFromOptionalValueO(configReader.autoLinkLevel, m_autoLinkLevel);
	UpdateValueFromOptionalValue(configReader.headersIncludePaths, m_headersIncludePaths);
	UpdateValueFromOptionalValue(configReader.moduleIncludePaths, m_moduleIncludePathList);
	UpdateValueFromOptionalValue(configReader.depFilePath, m_depFilePath);
	UpdateValueFromOptionalValue(configReader.moduleOutputFilePath, m_moduleOutputFilePath);
	UpdateValueFromOptionalValueO(configReader.moduleGenerateEnabled, m_isModuleGenerationEnabled);
	UpdateValueFromOptionalValueO(configReader.templateEnabled, m_isTemplateEnabled);
	UpdateValueFromOptionalValue(configReader.outputDirTemplate, m_outputDirTemplate);
	UpdateValueFromOptionalValue(configReader.qmlOutputDirTemplate, m_outputQmlDirTemplate);
	UpdateValueFromOptionalValue(configReader.includePathTemplate, m_includePathTemplate);

	return true;
}


// reimplemented (imtsdl::ISdlEditableProcessArgumentsParser)

void CSdlProcessArgumentsParserComp::SetSchemaFilePath(const QString& schemaFilePath)
{
	m_schemaFilePath = schemaFilePath;
}


void CSdlProcessArgumentsParserComp::SetOutputDirectoryPath(const QString& outputDirectoryPath)
{
	m_outputDirectoryPath = outputDirectoryPath;
}


void CSdlProcessArgumentsParserComp::SetNamespace(const QString& aNamespace)
{
	m_namespace = aNamespace;
}


void CSdlProcessArgumentsParserComp::SetNamespacePrefix(const QString& namespacePrefix)
{
	m_namespacePrefix = namespacePrefix;
}


void CSdlProcessArgumentsParserComp::SetDependenciesModeEnabled(bool enabled)
{
	m_isDependenciesMode = enabled;
}


void CSdlProcessArgumentsParserComp::SetGenerateModeEnabled(bool enabled)
{
	m_isGenerateMode = enabled;
}


void CSdlProcessArgumentsParserComp::SetEnabledModificators(const QStringList& modificators)
{
	m_usedModificators = modificators;
}


void CSdlProcessArgumentsParserComp::SetEnabledAllModificators(bool enable)
{
	m_useAllModificators = enable;
}


void CSdlProcessArgumentsParserComp::SetCppEnabled(bool enabled)
{
	m_cppEnabled = enabled;
}


void CSdlProcessArgumentsParserComp::SetQmlEnabled(bool enabled)
{
	m_qmlEnabled = enabled;
}


void CSdlProcessArgumentsParserComp::SetGqlEnabled(bool enabled)
{
	m_gqlEnabled = enabled;
}


void CSdlProcessArgumentsParserComp::SetSchemaDependencyModeEnabled(bool enabled)
{
	m_schemaDependencyModeEnabled = enabled;
}


void CSdlProcessArgumentsParserComp::SetBaseClassList(const QMap<QString, QString>& baseClassList)
{
	m_baseClassList = baseClassList;
}


void CSdlProcessArgumentsParserComp::SetJoinRules(const QMap<QString, QString>& rules)
{
	m_joinRules = rules;
}


void CSdlProcessArgumentsParserComp::SetIncludePaths(const QStringList& includePaths)
{
	m_includePaths = includePaths;
}


void CSdlProcessArgumentsParserComp::SetGeneratorType(GeneratorType type)
{
	m_generatorType = type;
}


void CSdlProcessArgumentsParserComp::SetAutoJoinEnabled(bool enabled)
{
	m_autoJoinEnabled = enabled;
}


void CSdlProcessArgumentsParserComp::SetAutoLinkLevel(AutoLinkLevel level)
{
	m_autoLinkLevel = level;
}


void CSdlProcessArgumentsParserComp::SetHeadersIncludePaths(const QStringList& includePaths)
{
	m_headersIncludePaths = includePaths;
	}


void CSdlProcessArgumentsParserComp::SetModuleIncludePaths(const QStringList& includePaths)
{
	m_moduleIncludePathList = includePaths;
}


void CSdlProcessArgumentsParserComp::SetDepFilePath(const QString& path)
{
	m_depFilePath = path;
}


void CSdlProcessArgumentsParserComp::SetModuleOutputFilePath(const QString& path)
{
	m_moduleOutputFilePath = path;
}


void CSdlProcessArgumentsParserComp::SetModuleGenerateEnabled(bool enabled)
{
	m_isModuleGenerationEnabled = enabled;
}


void CSdlProcessArgumentsParserComp::SetTemplateEnabled(bool enabled)
{
	m_isTemplateEnabled = enabled;
}


void CSdlProcessArgumentsParserComp::SetTemplateIncludePath(const QString& templateIncludePath)
{
	m_includePathTemplate = templateIncludePath;
}


void CSdlProcessArgumentsParserComp::SetTemplateOutputPath(const QString& templateOutputPath)
{
	m_outputDirTemplate = templateOutputPath;
}


// pseudo-implemented (protected) methods

QList<QCommandLineOption> CSdlProcessArgumentsParserComp::PrepareCommandLineOptions()
{
	return QList<QCommandLineOption>();
}


bool CSdlProcessArgumentsParserComp::ProcessCommandLineOptions(const QCommandLineParser& /*commandLineParser*/)
{
	return true;
}


} // namespace imtsdlgen


