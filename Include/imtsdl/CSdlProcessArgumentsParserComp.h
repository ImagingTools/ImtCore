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

private:
	QString m_schemaFilePath;
	QString m_outputDirectoryPath;
	QString m_headerFileExctention;
	QString m_sourceCodeFileExctention;
	QString m_namespace;
	bool m_isGenerateMode;
	bool m_isDependenciesMode;
};


} // namespace imtsdl


