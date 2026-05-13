// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QCommandLineOption>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtsdl/ISdlEditableProcessArgumentsParser.h>


class QCommandLineParser;


namespace imtsdl
{


class CSdlProcessArgumentsParserComp:
			public ilog::CLoggerComponentBase,
			virtual public ISdlEditableProcessArgumentsParser
{


public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSdlProcessArgumentsParserComp)
		I_REGISTER_INTERFACE(imtsdl::ISdlProcessArgumentsParser)
		I_REGISTER_INTERFACE(imtsdl::ISdlEditableProcessArgumentsParser)
	I_END_COMPONENT;

	// reimplemented (imtsdl::ISdlProcessArgumentsParser)
	virtual bool SetArguments(const QStringList& arguments) override;
	virtual QString GetSchemaFilePath() const override;
	virtual QString GetOutputDirectoryPath() const override;
	virtual QString GetNamespace() const override;
	virtual QString GetNamespacePrefix() const override;
	virtual bool IsGenerateMode() const override;
	virtual bool IsDependenciesMode() const override;
	virtual bool IsModificatorEnabled(const QString& modificatorName) const override;
	virtual bool IsQmlEnabled() const override;
	virtual bool IsCppEnabled() const override;
	virtual bool IsGqlEnabled() const override;
	virtual bool IsSchemaDependencyModeEnabled() const override;
	virtual QMap<QString, QString> GetBaseClassList() const override;
	virtual QStringList GetIncludePaths() const override;
	virtual GeneratorType GetGeneratorType() const override;
	virtual QStringList GetHeadersIncludePaths() const override;
	virtual QString GetDepFilePath() const override;
	virtual bool IsTemplateEnabled() const override;
	virtual QString GetTemplateIncludePath() const override;
	virtual QString GetTemplateOutputPath() const override;
	virtual QString GetTemplateQmlOutputPath() const override;
	virtual CppGenerationMode GetCppGenerationMode() const override;
	virtual TypenameWriteMode GetTypenameWriteMode() const override;

	// reimplemented (imtsdl::ISdlEditableProcessArgumentsParser)
	virtual bool ReadFromSettings(const QString& settingsFilePath) override;
	virtual void SetSchemaFilePath(const QString& schemaFilePath) override;
	virtual void SetOutputDirectoryPath(const QString& outputDirectoryPath) override;
	virtual void SetNamespace(const QString& aNamespace) override;
	virtual void SetNamespacePrefix(const QString& namespacePrefix) override;
	virtual void SetDependenciesModeEnabled(bool enabled = true) override;
	virtual void SetGenerateModeEnabled(bool enabled = true) override;
	virtual void SetEnabledModificators(const QStringList& modificators) override;
	virtual void SetEnabledAllModificators(bool enable = true) override;
	virtual void SetCppEnabled(bool enabled = true) override;
	virtual void SetQmlEnabled(bool enabled = true) override;
	virtual void SetGqlEnabled(bool enabled = true) override;
	virtual void SetSchemaDependencyModeEnabled(bool enabled = true) override;
	virtual void SetBaseClassList(const QMap<QString, QString>& baseClassList) override;
	virtual void SetIncludePaths(const QStringList & includePaths) override;
	virtual void SetGeneratorType(GeneratorType type) override;
	virtual void SetHeadersIncludePaths(const QStringList& includePaths) override;
	virtual void SetDepFilePath(const QString& path) override;
	virtual void SetTemplateEnabled(bool enabled) override;
	virtual void SetTemplateIncludePath(const QString& templateIncludePath) override;
	virtual void SetTemplateOutputPath(const QString& templateOutputPath) override;
	virtual void SetTypenameWriteMode(TypenameWriteMode mode) override;


protected:
	/**
		\brief Prepares command line options
		\note This method will be invoked PRIOR to the options being parsed.
		\returns a list of all acceptable options
		\note inherits classes SHOULD NOT call base method. Default implementation returns nothing.
	 */
	virtual QList<QCommandLineOption> PrepareCommandLineOptions();

	/**
		\brief Processing parsed options
		\note This method will be invoked AFTER a commandLineParser process the input parameters.
		\note inherits classes SHOULD NOT call base method. Default implementation does nothing.
	 */
	virtual bool ProcessCommandLineOptions(const QCommandLineParser& commandLineParser);

private:
	QString m_schemaFilePath;
	QString m_outputDirectoryPath;
	QString m_headerFileExctention;
	QString m_sourceCodeFileExctention;
	QString m_namespace;
	QString m_namespacePrefix;
	QStringList m_usedModificators;
	std::optional<bool> m_isGenerateMode;
	std::optional<bool> m_isDependenciesMode;
	std::optional<bool> m_useAllModificators;
	std::optional<bool> m_notUseModificators;
	std::optional<bool> m_cppEnabled;
	std::optional<bool> m_qmlEnabled;
	std::optional<bool> m_gqlEnabled;
	std::optional<bool> m_schemaDependencyModeEnabled;
	QMap<QString, QString> m_baseClassList;
	QStringList m_includePaths;
	std::optional<GeneratorType> m_generatorType;
	QStringList m_headersIncludePaths;
	QString m_depFilePath;
	std::optional<bool> m_isTemplateEnabled;
	QString m_outputDirTemplate;
	QString m_outputQmlDirTemplate;
	QString m_includePathTemplate;
	std::optional<CppGenerationMode> m_cppGenerationMode;
	std::optional<TypenameWriteMode> m_typenameWriteMode;
};


} // namespace imtsdl


