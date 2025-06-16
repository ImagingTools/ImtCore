#pragma once


// Qt includes
#include <QtCore/QCommandLineOption>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtsdl/ISdlEditableProcessArgumentsParser.h>


class QCommandLineParser;


namespace imtsdlgen
{


class CSdlProcessArgumentsParserComp:
			public ilog::CLoggerComponentBase,
			virtual public imtsdl::ISdlEditableProcessArgumentsParser
{


public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSdlProcessArgumentsParserComp)
		I_REGISTER_INTERFACE(imtsdl::ISdlProcessArgumentsParser)
		I_REGISTER_INTERFACE(imtsdl::ISdlEditableProcessArgumentsParser)
	I_END_COMPONENT;

	CSdlProcessArgumentsParserComp();

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
	virtual QMap<QString, QString> GetJoinRules() const override;
	virtual QStringList GetIncludePaths() const override;
	virtual GeneratorType GetGeneratorType() const override;
	virtual bool IsAutoJoinEnabled() const override;
	virtual AutoLinkLevel GetAutoLinkLevel() const override;
	virtual QStringList GetHeadersIncludePaths() const override;
	virtual QStringList GetModuleIncludePaths() const override;
	virtual QString GetDepFilePath() const override;
	virtual QString GetModuleOutputFilePath() const override;
	virtual bool IsModuleGenerateEnabled() const override;

	// reimplemented (imtsdl::ISdlEditableProcessArgumentsParser)
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
	virtual void SetJoinRules(const QMap<QString, QString>& rules) override;
	virtual void SetIncludePaths(const QStringList & includePaths) override;
	virtual void SetGeneratorType(GeneratorType type) override;
	virtual void SetAutoJoinEnabled(bool enable = true) override;
	virtual void SetAutoLinkLevel(AutoLinkLevel level) override;
	virtual void SetHeadersIncludePaths(const QStringList& includePaths) override;
	virtual void SetModuleIncludePaths(const QStringList& includePaths) override;
	virtual void SetDepFilePath(const QString& path) override;
	virtual void SetModuleOutputFilePath(const QString& path) override;
	virtual void SetModuleGenerateEnabled(bool enable = true) override;


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
	bool m_isGenerateMode;
	bool m_isDependenciesMode;
	bool m_useAllModificators;
	bool m_notUseModificators;
	bool m_cppEnabled;
	bool m_qmlEnabled;
	bool m_gqlEnabled;
	bool m_schemaDependencyModeEnabled;
	QMap<QString, QString> m_baseClassList;
	QMap<QString, QString> m_joinRules;
	QStringList m_includePaths;
	GeneratorType m_generatorType;
	bool m_autoJoinEnabled;
	AutoLinkLevel m_autolinkLevel;
	QStringList m_headersIncludePaths;
	QStringList m_moduleIncludePathList;
	QString m_depFilePath;
	QString m_moduleOutputFilePath;
	bool m_isModuleGenerationEnabled;
};


} // namespace imtsdlgen


