#pragma once


// Qt includes
#include <QtCore/QCommandLineOption>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtsdl/CSdlProcessArgumentsParserComp.h>
#include <imtsdlgencpp/ISdlProcessArgumentsParser.h>


namespace imtsdlgencpp
{

/// \todo remove it
class CSdlProcessArgumentsParserComp:
			public imtsdl::CSdlProcessArgumentsParserComp,
			virtual public imtsdlgencpp::ISdlProcessArgumentsParser
{


public:
	typedef imtsdl::CSdlProcessArgumentsParserComp BaseClass;

	I_BEGIN_COMPONENT(CSdlProcessArgumentsParserComp)
		I_REGISTER_INTERFACE(imtsdl::ISdlProcessArgumentsParser)
		I_REGISTER_INTERFACE(ISdlProcessArgumentsParser)
	I_END_COMPONENT;

	CSdlProcessArgumentsParserComp();

	// reimplemented (imtsdlgencpp::ISdlProcessArgumentsParser)
	virtual quint16 GetGenerationVersion() const override;


protected:
	// reimplemented (imtsdlgencpp::CSdlProcessArgumentsParserComp)
	virtual QList<QCommandLineOption> PrepareCommandLineOptions() override;
	virtual bool ProcessCommandLineOptions(const QCommandLineParser& commandLineParser) override;

private:
	quint16 m_generationVersion;
	std::unique_ptr<QCommandLineOption> m_generationCommandLinePtr;
};


} // namespace imtsdlgencpp


