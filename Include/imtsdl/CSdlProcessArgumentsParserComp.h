#pragma once


// Acf includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IParamsSet.h>

// imtsdl includes
#include <imtsdl/ISdlProcessArgumentsParser.h>


namespace imtsdl
{


class CSdlProcessArgumentsParserComp:
			public ilog::CLoggerComponentBase,
			virtual public ISdlProcessArgumentsParser
{


public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSdlProcessArgumentsParserComp)
		I_REGISTER_INTERFACE(ISdlProcessArgumentsParser)
	I_END_COMPONENT;

	CSdlProcessArgumentsParserComp();

	// reimplemented (ISdlProcessArgumentsParser)
	virtual bool SetArguments(int argc, char** argv) override;
	virtual QString GetSchemaFilePath() const override;
	virtual QString GetOutputDirectoryPath() const override;
	virtual QString GetNamespace() const override;
	virtual bool IsGenerateMode() const override;
	virtual bool IsDependenciesMode() const override;
	virtual bool IsModificatorEnabled(const QString& modificatorName) const override;
	virtual bool IsQmlEnabled() const override;
	virtual bool IsCppEnabled() const override;
	virtual bool IsGqlEnabled() const override;
	virtual QMap<QString, QString> GetBaseClassList() const override;

private:
	QString m_schemaFilePath;
	QString m_outputDirectoryPath;
	QString m_headerFileExctention;
	QString m_sourceCodeFileExctention;
	QString m_namespace;
	QStringList m_usedModificators;
	bool m_isGenerateMode;
	bool m_isDependenciesMode;
	bool m_useAllModificators;
	bool m_notUseModificators;
	bool m_cppEnabled;
	bool m_qmlEnabled;
	bool m_gqlEnabled;
	QMap<QString, QString> m_baseClassList;
};


} // namespace imtsdl


