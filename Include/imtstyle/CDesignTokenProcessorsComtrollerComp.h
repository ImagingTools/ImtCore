#pragma once

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtstyle/IDesignTokenProcessor.h>
#include <imtstyle/IDesignTokenArgumentsParser.h>


namespace imtstyle
{

class CDesignTokenProcessorsComtrollerComp: public ilog::CLoggerComponentBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDesignTokenProcessorsComtrollerComp);
		I_ASSIGN(m_argumentParserAttrPtr, "ArgumentsParser", "Arguments Parser", true, "IDesignTokenArgumentsParser")
		I_ASSIGN_MULTI_0(m_designTokenProcessors, "DesignTokenProcessors", "Arguments Parser", true)
	I_END_COMPONENT;



protected:
	// reimplemented (ilog::CLoggerComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_MULTIREF(IDesignTokenProcessor, m_designTokenProcessors);
	I_REF(IDesignTokenArgumentsParser, m_argumentParserAttrPtr);

};


} // namespace imtstyle

