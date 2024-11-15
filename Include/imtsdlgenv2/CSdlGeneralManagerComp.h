#pragma once


//Qt includes
#include <QtCore/QFile>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iproc/IProcessor.h>

// ImtCore includes
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdlgen/CSdlGeneralManagerComp.h>


namespace imtsdlgenv2
{


class CSdlGeneralManagerComp: public imtsdlgen::CSdlGeneralManagerComp
{


public:
	typedef imtsdlgen::CSdlGeneralManagerComp BaseClass;

	I_BEGIN_COMPONENT(CSdlGeneralManagerComp)
		I_ASSIGN(m_sdlArgumentParserV2CompPtr, "ArgumentParserV2", "Argument parser 2nd generation", false, "ArgumentParser")
		I_ASSIGN_MULTI_0(m_sdlV2ProcessorsCompListPtr, "SdlProcessorV2List", "The list of SDL processors 2nd generation", true)
	I_END_COMPONENT;

protected:
	// reimplemented (imtsdlgen::CSdlGeneralManagerComp)
	bool CreateCode() override;

private:
	I_REF(imtsdl::ISdlProcessArgumentsParser, m_sdlArgumentParserV2CompPtr);
	I_MULTIREF(iproc::IProcessor, m_sdlV2ProcessorsCompListPtr);
};


} // namespace imtsdlgenv2







