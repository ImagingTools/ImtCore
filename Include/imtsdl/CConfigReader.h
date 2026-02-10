// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>
#include <QtCore/QMap>

// ImtCore includes
#include <imtsdl/ISdlProcessArgumentsParser.h>


namespace imtsdl
{


/**
	The SDL Generator config file reader.
*/
class CConfigReader: virtual public istd::IPolymorphic
{

public:
	[[nodiscard]] bool ReadFromFile(const QString& filePath);

	std::optional<QString> schemaFilePath;
	std::optional<QString> outputDirectoryPath;
	std::optional<QString> namespaceValue;
	std::optional<QString> namespacePrefix;
	std::optional<bool> dependenciesMode;
	std::optional<bool> generateMode;
	std::optional<QStringList> enabledModificators;
	std::optional<bool> useAllModificators;
	std::optional<bool> cppEnabled;
	std::optional<bool> qmlEnabled;
	std::optional<bool> gqlEnabled;
	std::optional<bool> schemaDependencyModeEnabled;
	std::optional<QMap<QString, QString>> baseClassList;
	std::optional<QStringList> includePaths;
	std::optional<imtsdl::ISdlProcessArgumentsParser::GeneratorType> generatorType;
	std::optional<QStringList> headersIncludePaths;
	std::optional<QString> depFilePath;
	std::optional<bool> templateEnabled;
	std::optional<QString> outputDirTemplate;
	std::optional<QString> qmlOutputDirTemplate;
	std::optional<QString> includePathTemplate;
	std::optional<ISdlProcessArgumentsParser::TypenameWriteMode> typenameWriteMode;
};


} // namespace imtsdl

