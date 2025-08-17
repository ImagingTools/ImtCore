#pragma once


//Qt includes
#include <QtCore/QFile>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iproc/TSyncProcessorWrap.h>

// ImtCore includes
#include <imtsdl/ISdlProcessArgumentsParser.h>


namespace imtsdlgencpp
{

/// \todo rename it to CSdlCppManager
class CCxxProcessorsManagerComp:public ilog::CLoggerComponentBase,
							public iproc::CSyncProcessorBase
{


public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCxxProcessorsManagerComp)
		I_REGISTER_INTERFACE(iproc::IProcessor)
		I_ASSIGN(m_sdlArgumentParserCompPtr, "ArgumentParser", "Argument parser generation", true, "ArgumentParser")
		I_ASSIGN_MULTI_0(m_sdlV2ProcessorsCompListPtr, "SdlProcessorV2List", "The list of CXX processors generation", true)
	I_END_COMPONENT;

protected:
	// reimplemented (iproc::IProcessor)
	virtual TaskState DoProcessing(
		const iprm::IParamsSet* paramsPtr,
		const istd::IPolymorphic* inputPtr,
		istd::IChangeable* outputPtr,
		ibase::IProgressManager* progressManagerPtr = nullptr) override;

private:
	I_REF(imtsdl::ISdlProcessArgumentsParser, m_sdlArgumentParserCompPtr);
	I_MULTIREF(iproc::IProcessor, m_sdlV2ProcessorsCompListPtr);
};


} // namespace imtsdlgencpp







