#pragma once

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iproc/IProcessor.h>

// ImtCore includes
#include <imtddl/IDdlCodeCreatorArgumentsParser.h>


namespace imtddl
{

class CDdlCodeCreatorProcessorsControllerComp: public ilog::CLoggerComponentBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDdlCodeCreatorProcessorsControllerComp);
		I_ASSIGN(m_argumentParserAttrPtr, "ArgumentsParser", "Arguments Parser", true, "IDesignTokenArgumentsParser")
		I_ASSIGN_MULTI_0(m_ddlProcessors, "DesignTokenProcessors", "Arguments Parser", true)
	I_END_COMPONENT;



protected:
	// reimplemented (ilog::CLoggerComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_REF(IDdlCodeCreatorArgumentsParser, m_argumentParserAttrPtr);
	I_MULTIREF(iproc::IProcessor, m_ddlProcessors);
};


} // namespace imtddl

