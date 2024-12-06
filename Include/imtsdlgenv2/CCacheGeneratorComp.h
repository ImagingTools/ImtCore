#pragma once


// ACF includes
#include <iproc/TSyncProcessorCompBase.h>

// ImtCore includes
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdl/ISdlTypeListProvider.h>
#include <imtsdl/ICacheMultiManager.h>
#include <imtsdl/CSdlTools.h>
#include <imtsdlgenv2/CSdlGenTools.h>


namespace imtsdlgenv2
{

/**
	A base C++ class generator of SDL types
*/
class CCacheGeneratorComp:
			public iproc::CSyncProcessorCompBase,
			private imtsdl::CSdlTools,
			private CSdlGenTools
{

public:
	typedef iproc::CSyncProcessorCompBase BaseClass;
	typedef imtsdl::CSdlTools BaseClass2;
	typedef CSdlGenTools BaseClass3;

	I_BEGIN_COMPONENT(CCacheGeneratorComp)
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentParser", "Command line process argument parser", true, "ArgumentParser")
		I_ASSIGN(m_sdlTypeListCompPtr, "SdlTypeListProvider", "SDL types used to create a code", true, "SdlTypeListProvider")
		I_ASSIGN(m_schemaParamsCompPtr, "CurrentSchemaParams", "Current schema parameters, that contains additional options", false, "CurrentSchemaParams")
		I_ASSIGN(m_cacheControllerCompPtr, "CacheController", "Cache controller, used to create a cache for current scheme", true, "CacheController")

	I_END_COMPONENT

	//reimplemented(iproc::IProcessor)
	virtual int DoProcessing(
				const iprm::IParamsSet* paramsPtr,
				const istd::IPolymorphic* inputPtr,
				istd::IChangeable* outputPtr,
				ibase::IProgressManager* progressManagerPtr = NULL) override;

private:
	I_REF(imtsdl::ISdlProcessArgumentsParser, m_argumentParserCompPtr);
	I_REF(imtsdl::ISdlTypeListProvider, m_sdlTypeListCompPtr);
	I_REF(iprm::IParamsSet, m_schemaParamsCompPtr);
	I_REF(imtsdl::ICacheController, m_cacheControllerCompPtr);
};


} // namespace imtsdlgenv2

