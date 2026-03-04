// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtsdl/CConfigReader.h>


//Qt includes
#include <QtCore/QSettings>
#include <QtCore/QMetaEnum>
#include <QtCore/QFileInfo>


namespace imtsdl
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


template <class T>
bool ConvertStringToEnum(const QString& value, T& output)
{
	bool retVal = false;
	QMetaEnum metaEnum = QMetaEnum::fromType<T>();
	int enumValue = metaEnum.keyToValue(value.toStdString().c_str(), &retVal);

	if (!retVal){
		QStringList expectedValues;
		for (int i = 0; i < metaEnum.keyCount(); i++) {
			expectedValues << metaEnum.key(i);
		}

		qCritical() << "Unexpected enum value. Expected one of: " << expectedValues.join("; ");
	}
	else {
		output = T(enumValue);
	}

	return retVal;
}


template <class T>
bool SetEnumVariableFromConfig(const QSettings& configuration, const QString& valueName, std::optional<T>& output)
{
	QString typeValue;
	SetVariableFromConfig(configuration, valueName, typeValue);
	if (!typeValue.isEmpty()){
		T tempVal;
		bool isValid = ConvertStringToEnum(typeValue, tempVal);
		if (!isValid){
			qCritical() << "Configuration error. Unexpected" << valueName << "value. Actual: " << typeValue;

			return false;
		}

		output = tempVal;
	}

	return true;
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
	SetVariableFromConfig(configuration, QStringLiteral("include"), includePaths);
	SetVariableFromConfig(configuration, QStringLiteral("include-headers"), headersIncludePaths);
	SetVariableFromConfig(configuration, QStringLiteral("DEPFILE"), depFilePath);
	SetVariableFromConfig(configuration, QStringLiteral("CPP"), cppEnabled);
	SetVariableFromConfig(configuration, QStringLiteral("QML"), qmlEnabled);
	SetVariableFromConfig(configuration, QStringLiteral("GQL"), gqlEnabled);
	SetVariableFromConfig(configuration, QStringLiteral("schema-dependencies"), schemaDependencyModeEnabled);
	SetVariableFromConfig(configuration, QStringLiteral("generate-module"), generateMode);
	
	if (!SetEnumVariableFromConfig(configuration, QStringLiteral("typename-mode"), typenameWriteMode)){
		return false;
	}

	if (!SetEnumVariableFromConfig(configuration, QStringLiteral("generator"), generatorType)){
		return false;
	}

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
	SetVariableFromConfig(configuration, QStringLiteral("Template/qml-output"), qmlOutputDirTemplate);
	SetVariableFromConfig(configuration, QStringLiteral("Template/include-path"), includePathTemplate);

	return true;
}


// public methods


} // namespace imtsdl

