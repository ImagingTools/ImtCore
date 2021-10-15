#pragma once


// Qt includes
#include <QtCore/QtCore>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IParamsSet.h>
#include <istd/CSystem.h>

// ImtCore includes
#include <imtdesign/IDesignTokenProcessor.h>
#include <imtdesign/IDesignTokenArgumentsParser.h>
#include <imtdesign/IDesignTokenFileParser.h>
#include <imtdesign/IDesignTokenQrcUpdater.h>


namespace imtdesign
{

class CDesignTokenIconProcessorComp: public ilog::CLoggerComponentBase, public IDesignTokenProcessor
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDesignTokenIconProcessorComp);
		I_REGISTER_INTERFACE(IDesignTokenProcessor);
		I_ASSIGN(m_argumentParserAttrPtr, "ArgumentsParser", "Arguments Parser", true, "IDesignTokenArgumentsParser")
		I_ASSIGN(m_designTokenFileParserAttrPtr, "DesignTokenFileParser", "Design token file parser", true, "IDesignTokenFileParser")
	I_END_COMPONENT;

	// reimplemented (IDesignTokenProcessor)
	int Exec() override;
	QByteArray GetHelpString() const override;


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

	QByteArray m_templateIconColor;
	QByteArray m_normalColor;
	QByteArray m_offNormalColor;
	QByteArray m_offDisabledColor;
	QByteArray m_offActiveColor;
	QByteArray m_offSelectedColor;
	QByteArray m_onNormalColor;
	QByteArray m_onDisabledColor;
	QByteArray m_onActiveColor;
	QByteArray m_onSelectedColor;

	QByteArray m_inputDirName;
	QByteArray m_inputFileName;
	QByteArray m_outputDirName;
	QByteArray m_projectName;
	QFileInfo m_designTokenFileInfo;


private:
	/**
		sets new color in single image
	*/
	bool SetColor(const QByteArray& fileName, const QByteArray& outputFileName, const QByteArray& replacedColor, const QByteArray& reolacebleColor = QByteArray()) const;

	/**
		sets all available colors for single images
	*/
	bool SetColorForAllModeState(const QByteArray& fileName, const QByteArray& outputDirName) const;
	/**
		sets all colors for all images in directory
	*/
	bool SetColorAllFilesInDir(const QByteArray& inputDirName, const QByteArray& outputDirName) const;
	/**
		Checks for ignore some files e.g.? if it has suffix
	*/
	bool IgnoreFile(const QFileInfo& fileInfo) const;

};


} // namespace imtdesign

