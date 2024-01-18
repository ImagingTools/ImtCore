#pragma once


// Acf includes
#include <ilog/TLoggerCompWrap.h>

// imtsdl includes
#include <imtsdl/imtsdl.h>
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
	I_END_COMPONENT;

	// reimplemented (ISdlProcessArgumentsParser)
	virtual bool SetArguments(int argc, char** argv) override;
	virtual QString GetSchemaFilePath() const override;
	virtual QString GetOutputDirectoryPath() const override;

private:
	QString m_schemaFilePath;
	QString m_outputDirectoryPath;
	QString m_headerFileExctention;
	QString m_sourceCodeFileExctention;
};


} // namespace imtsdl


