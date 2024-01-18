#pragma once


//Qt includes
#include <QtCore/QFile>

// Acf includes
#include <ilog/TLoggerCompWrap.h>
#include <iproc/IProcessor.h>

// imtsdl includes
#include <imtsdl/ISdlProcessArgumentsParser.h>


namespace imtsdl
{


class CSdlGeneralManagerComp: public ilog::CLoggerComponentBase
{


public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSdlGeneralManagerComp)
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentsParser", "Arguments Parser", true, "ISdlCodeCreatorArgumentsParser")
		I_ASSIGN_MULTI_0(m_sdlProcessorsCompListPtr, "SdlProcessorList", "The list of SDL processors", true)
	I_END_COMPONENT;

protected:
	virtual void OnComponentCreated() override;

private:
	I_REF(ISdlProcessArgumentsParser, m_argumentParserCompPtr);
	I_REF(iproc::IProcessor, m_sdlParserCompPtr);
	I_MULTIREF(iproc::IProcessor, m_sdlProcessorsCompListPtr);
};


} // namespace imtsdl







