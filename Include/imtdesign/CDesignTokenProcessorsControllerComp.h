// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtdesign/IDesignTokenProcessor.h>
#include <imtdesign/IDesignTokenArgumentsParser.h>


namespace imtdesign
{

class CDesignTokenProcessorsControllerComp: public ilog::CLoggerComponentBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDesignTokenProcessorsControllerComp);
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentsParser", "Arguments Parser", true, "IDesignTokenArgumentsParser")
		I_ASSIGN_MULTI_0(m_designTokenProcessors, "DesignTokenProcessors", "Arguments Parser", true)
	I_END_COMPONENT;



protected:
	// reimplemented (ilog::CLoggerComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_MULTIREF(IDesignTokenProcessor, m_designTokenProcessors);
	I_REF(IDesignTokenArgumentsParser, m_argumentParserCompPtr);

};


} // namespace imtdesign

