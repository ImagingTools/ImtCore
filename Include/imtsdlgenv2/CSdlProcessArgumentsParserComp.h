#pragma once


// Qt includes
#include <QtCore/QCommandLineOption>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtsdlgen/CSdlProcessArgumentsParserComp.h>
#include <imtsdlgenv2/ISdlProcessArgumentsParser.h>


namespace imtsdlgenv2
{


class CSdlProcessArgumentsParserComp:
			public imtsdlgen::CSdlProcessArgumentsParserComp,
			virtual public imtsdlgenv2::ISdlProcessArgumentsParser
{


public:
	typedef imtsdlgen::CSdlProcessArgumentsParserComp BaseClass;

	I_BEGIN_COMPONENT(CSdlProcessArgumentsParserComp)
		I_REGISTER_INTERFACE(imtsdl::ISdlProcessArgumentsParser)
		I_REGISTER_INTERFACE(imtsdlgenv2::ISdlProcessArgumentsParser)
	I_END_COMPONENT;

	CSdlProcessArgumentsParserComp();

	// reimplemented (imtsdlgenv2::ISdlProcessArgumentsParser)
	virtual quint16 GetGenerationVersion() const override;


protected:
	// reimplemented (imtsdlgen::CSdlProcessArgumentsParserComp)
	virtual QList<QCommandLineOption> PrepareCommandLineOptions() override;
	virtual bool ProcessCommandLineOptions(const QCommandLineParser& commandLineParser) override;

private:
	quint16 m_generationVersion;
	std::unique_ptr<QCommandLineOption> m_generationCommandLinePtr;
};


} // namespace imtsdlgenv2


