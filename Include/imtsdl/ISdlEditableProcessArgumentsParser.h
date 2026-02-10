// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtsdl/ISdlProcessArgumentsParser.h>


namespace imtsdl
{

/// an editable version of a \c ISdlProcessArgumentsParser \note SHOULD be used only for tests
class ISdlEditableProcessArgumentsParser: virtual public ISdlProcessArgumentsParser
{
public:
	virtual bool ReadFromSettings(const QString& settingsPath) = 0;

	virtual void SetSchemaFilePath(const QString& schemaFilePath) = 0;
	virtual void SetOutputDirectoryPath(const QString& outputDirectoryPath) = 0;
	virtual void SetNamespace(const QString& aNamespace) = 0;
	virtual void SetNamespacePrefix(const QString& namespacePrefix) = 0;
	virtual void SetDependenciesModeEnabled(bool enabled = true) = 0;
	virtual void SetGenerateModeEnabled(bool enabled = true) = 0;
	virtual void SetEnabledModificators(const QStringList& modificators) = 0;
	virtual void SetEnabledAllModificators(bool enable = true) = 0;
	virtual void SetCppEnabled(bool enabled = true) = 0;
	virtual void SetQmlEnabled(bool enabled = true) = 0;
	virtual void SetGqlEnabled(bool enabled = true) = 0;
	virtual void SetSchemaDependencyModeEnabled(bool enabled = true) = 0;
	virtual void SetBaseClassList(const QMap<QString, QString>& baseClassList) = 0;
	virtual void SetIncludePaths(const QStringList & includePaths) = 0;
	virtual void SetGeneratorType(GeneratorType type) = 0;
	virtual void SetHeadersIncludePaths(const QStringList& includePaths) = 0;
	virtual void SetDepFilePath(const QString& path) = 0;
	virtual void SetTypenameWriteMode(TypenameWriteMode mode) = 0;

	// template
	virtual void SetTemplateEnabled(bool enable = true) = 0;
	virtual void SetTemplateIncludePath(const QString& templateIncludePath) = 0;
	virtual void SetTemplateOutputPath(const QString& templateOutputPath) = 0;
};


} // namespace imtsdl


