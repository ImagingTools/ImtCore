#include <QtCore/qfileinfo.h>
#include <imtsdlgen/CConfigReader.h>


//Qt includes
#include <QtCore/QSettings>


namespace imtsdlgen
{


// static helpers
void SetVariableFromConfig(const QSettings& configuration, const QString& valueName, std::optional<QString>& value)
{
	if (configuration.contains(valueName)){
		value = configuration.value(valueName).toString();
	}
}


void SetVariableFromConfig(const QSettings& configuration, const QString& valueName, std::optional<bool>& value)
{
	if (configuration.contains(valueName)){
		value = configuration.value(valueName).toBool();
	}
}


void SetVariableFromConfig(const QSettings& configuration, const QString& valueName, std::optional<QStringList>& value)
{
	if (configuration.contains(valueName)){
		value = configuration.value(valueName).toStringList();
	}
}


void SetVariableFromConfig(const QSettings& configuration, const QString& valueName, QString& value)
{
	if (configuration.contains(valueName)){
		value = configuration.value(valueName).toString();
	}
}



// public methods

bool CConfigReader::ReadFromFile(const QString& filePath)
{
	QFileInfo configFile(filePath);
	if (!configFile.exists() || !configFile.isFile() || !configFile.isReadable()){
		qCritical() << "Config Reader: incorrect file path provided:" << filePath;

		return false;
	}

	QSettings configuration(filePath, QSettings::IniFormat);

	SetVariableFromConfig(configuration, QStringLiteral("schema-file"), schemaFilePath);
	SetVariableFromConfig(configuration, QStringLiteral("output-directory"), outputDirectoryPath);
	SetVariableFromConfig(configuration, QStringLiteral("namespace"), namespaceValue);
	SetVariableFromConfig(configuration, QStringLiteral("namespace-prefix"), namespacePrefix);
	SetVariableFromConfig(configuration, QStringLiteral("generate"), generateMode);
	SetVariableFromConfig(configuration, QStringLiteral("dependencies"), dependenciesMode);
	SetVariableFromConfig(configuration, QStringLiteral("modificators"), enabledModificators);
	SetVariableFromConfig(configuration, QStringLiteral("use-all-modificators"), useAllModificators);
	SetVariableFromConfig(configuration, QStringLiteral("join"), autoJoinEnabled);
	SetVariableFromConfig(configuration, QStringLiteral("include"), includePaths);
	SetVariableFromConfig(configuration, QStringLiteral("include-headers"), headersIncludePaths);
	SetVariableFromConfig(configuration, QStringLiteral("auto-join"), autoJoinEnabled);
	SetVariableFromConfig(configuration, QStringLiteral("extend"), moduleIncludePaths);
	SetVariableFromConfig(configuration, QStringLiteral("DEPFILE"), depFilePath);
	SetVariableFromConfig(configuration, QStringLiteral("module-out-path"), moduleOutputFilePath);
	SetVariableFromConfig(configuration, QStringLiteral("CPP"), cppEnabled);
	SetVariableFromConfig(configuration, QStringLiteral("QML"), qmlEnabled);
	SetVariableFromConfig(configuration, QStringLiteral("GQL"), gqlEnabled);
	SetVariableFromConfig(configuration, QStringLiteral("schema-dependencies"), schemaDependencyModeEnabled);
	SetVariableFromConfig(configuration, QStringLiteral("generate-module"), generateMode);

	if (configuration.contains(QStringLiteral("base-class"))){
		const QVariantMap baseClassMap = configuration.value(QStringLiteral("base-class")).toMap();
		if (!baseClassMap.isEmpty()){
			baseClassList.emplace();
			for (auto cfgMapIter = baseClassMap.cbegin(); cfgMapIter != baseClassMap.cend(); ++cfgMapIter){
				baseClassList->insert(cfgMapIter.key(), cfgMapIter->toString());
			}
		}
	}

	SetVariableFromConfig(configuration, QStringLiteral("Template/enable"), templateEnabled);
	SetVariableFromConfig(configuration, QStringLiteral("Template/output"), outputDirTemplate);

	QString generatorTypeValue;
	SetVariableFromConfig(configuration, QStringLiteral("generator"), generatorTypeValue);

	QString autoLinkLevelValue;
	SetVariableFromConfig(configuration, QStringLiteral("auto-link"), autoLinkLevelValue);


	return true;
}


// public methods


} // namespace imtsdlgen

