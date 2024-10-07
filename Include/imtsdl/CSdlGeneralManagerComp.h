#pragma once


//Qt includes
#include <QtCore/QFile>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iproc/IProcessor.h>

// ImtCore includes
#include <imtsdl/ISdlProcessArgumentsParser.h>


namespace imtsdl
{


class CSdlGeneralManagerComp: public ilog::CLoggerComponentBase
{


public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSdlGeneralManagerComp)
		I_ASSIGN(m_sdlArgumentParserCompPtr, "ArgumentParser", "Argument parser", true, "ArgumentParser")
		I_ASSIGN(m_sdlParserCompPtr, "SdlSchemaParser", "SDL schema Parser", true, "SdlSchemaParser")
		I_ASSIGN(m_sdlSchemaDependenciesCollectorCompPtr, "SdlSchemaDependenciesCollector", "Processor, used to collect a list of all schemas that affect the generated code", true, "SdlSchemaDependenciesCollector")
		I_ASSIGN_MULTI_0(m_sdlProcessorsCompListPtr, "SdlProcessorList", "The list of SDL processors", true)
	I_END_COMPONENT;

protected:
	virtual void OnComponentCreated() override;

private:
	I_REF(ISdlProcessArgumentsParser, m_sdlArgumentParserCompPtr);
	I_REF(iproc::IProcessor, m_sdlParserCompPtr);
	I_REF(iproc::IProcessor, m_sdlSchemaDependenciesCollectorCompPtr);
	I_MULTIREF(iproc::IProcessor, m_sdlProcessorsCompListPtr);
};


} // namespace imtsdl







