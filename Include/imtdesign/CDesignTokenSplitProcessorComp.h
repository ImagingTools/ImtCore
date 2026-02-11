// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QtCore>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtdesign/IDesignTokenProcessor.h>
#include <imtdesign/IDesignTokenArgumentsParser.h>
#include <imtdesign/IDesignTokenFileParser.h>


namespace imtdesign
{

class CDesignTokenSplitProcessorComp: public ilog::CLoggerComponentBase, public IDesignTokenProcessor
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDesignTokenSplitProcessorComp);
		I_REGISTER_INTERFACE(IDesignTokenProcessor);
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentsParser", "Arguments Parser", true, "IDesignTokenArgumentsParser")
		I_ASSIGN(m_designTokenFileParserCompPtr, "DesignTokenFileParser", "Design token file parser", true, "IDesignTokenFileParser")
	I_END_COMPONENT;

	// reimplemented (IDesignTokenProcessor)
	virtual int Exec() override;
	virtual QByteArray GetHelpString() const override;


private:
	I_REF(IDesignTokenArgumentsParser, m_argumentParserCompPtr);
	I_REF(IDesignTokenFileParser, m_designTokenFileParserCompPtr);

};


} // namespace imtdesign

