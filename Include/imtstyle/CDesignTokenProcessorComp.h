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

class CDesignTokenProcessorComp: public ilog::CLoggerComponentBase, public IDesignTokenProcessor
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDesignTokenProcessorComp);
		I_REGISTER_INTERFACE(IDesignTokenFileParser);
		I_ASSIGN(m_argumentParserAttrPtr, "ArgumentsParser", "Arguments Parser", true, "IDesignTokenArgumentsParser")
		I_ASSIGN(m_designTokenFileParserAttrPtr, "DesignTokenFileParser", "Design token file parser", true, "IDesignTokenFileParser")
		I_ASSIGN(m_paramSetAttrPtr, "ParamSet", "Param set", false, "IParamsSet")
	I_END_COMPONENT;

	int Exec() override;


protected:
	// reimplemented (ilog::CLoggerComponentBase)
	virtual void OnComponentCreated() override;

private:
	constexpr static const char* s_suffixOffNormal = "_Off_Normal";
	constexpr static const char* s_suffixOffDisabled = "_Off_Disabled";
	constexpr static const char* s_suffixOffActive = "_Off_Active";
	constexpr static const char* s_suffixOffSelected = "_Off_Selected";
	constexpr static const char* s_suffixOnNormal = "_On_Normal";
	constexpr static const char* s_suffixOnDisabled = "_On_Disabled";
	constexpr static const char* s_suffixOnActive = "_On_Active";
	constexpr static const char* s_suffixOnSelected = "_On_Selected";

	I_REF(IDesignTokenArgumentsParser, m_argumentParserAttrPtr);
	I_REF(IDesignTokenFileParser, m_designTokenFileParserAttrPtr);
	I_REF(iprm::IParamsSet, m_paramSetAttrPtr);


	QByteArray m_templateIconColor;
	QByteArray m_templatePrimaryColor;
	QByteArray m_templateSecondaryColor;
	QByteArray m_inputDirName;
	QByteArray m_inputFileName;
	QByteArray m_outputDirName;
	QFileInfo m_designTokenFileInfo;


private:
	///displays help
	int ShowHelp() const;
	/// sets new color in single image
	void SetColor(const QByteArray& fileName, const QByteArray& outputFileName, const QByteArray& replacedColor) const;
	/// sets all available colors for single images
	void SetColorForAllModeState(const QByteArray& fileName, const QByteArray& outputDirName) const;
	/// sets all colors for all images in directory
	void SetColorAllFilesInDir(const QByteArray& inputDirName, const QByteArray& outputDirName) const;
	/// Checks for ignore some files e.g.? if it has suffix
	bool IgnoreFile(const QFileInfo& fileInfo) const;

};


} // namespace imtstyle

