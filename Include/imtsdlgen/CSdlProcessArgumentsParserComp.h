#pragma once


// Qt includes
#include <QtCore/QCommandLineOption>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtsdl/ISdlProcessArgumentsParser.h>


class QCommandLineParser;


namespace imtsdlgen
{


class CSdlProcessArgumentsParserComp:
			public ilog::CLoggerComponentBase,
			virtual public imtsdl::ISdlProcessArgumentsParser
{


public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSdlProcessArgumentsParserComp)
		I_REGISTER_INTERFACE(imtsdl::ISdlProcessArgumentsParser)
	I_END_COMPONENT;

	CSdlProcessArgumentsParserComp();

	// reimplemented (imtsdl::ISdlProcessArgumentsParser)
	virtual bool SetArguments(int argc, char** argv) override;
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
};


} // namespace imtsdlgen


