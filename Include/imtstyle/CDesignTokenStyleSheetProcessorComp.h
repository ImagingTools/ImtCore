#pragma once


// Qt includes
#include <QtCore/QtCore>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtstyle/IDesignTokenProcessor.h>
#include <imtstyle/IDesignTokenArgumentsParser.h>
#include <imtstyle/IDesignTokenFileParser.h>


namespace imtstyle
{


class CDesignTokenStylesheetProcessorComp: public ilog::CLoggerComponentBase, public IDesignTokenProcessor
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDesignTokenStylesheetProcessorComp);
		I_REGISTER_INTERFACE(IDesignTokenFileParser);
		I_ASSIGN(m_argumentParserAttrPtr, "ArgumentsParser", "Arguments Parser", true, "IDesignTokenArgumentsParser")
		I_ASSIGN(m_designTokenFileParserAttrPtr, "DesignTokenFileParser", "Design token file parser", true, "IDesignTokenFileParser")
	I_END_COMPONENT;

	// reimplemented (IDesignTokenProcessor)
	int Exec() override;
	QByteArray GetHelpString() const override;


private:
	I_REF(IDesignTokenArgumentsParser, m_argumentParserAttrPtr);
	I_REF(IDesignTokenFileParser, m_designTokenFileParserAttrPtr);

	QByteArray m_inputDirName;
	QByteArray m_outputDirName;
	QByteArray m_designTokenFileInfo;

};


} // namespace imtstyle

