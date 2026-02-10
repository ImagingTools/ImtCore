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

class CDesignTokenIconProcessorComp: public ilog::CLoggerComponentBase, public IDesignTokenProcessor
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDesignTokenIconProcessorComp);
		I_REGISTER_INTERFACE(IDesignTokenProcessor);
		I_ASSIGN(m_argumentParserCompPtr, "ArgumentsParser", "Arguments Parser", true, "IDesignTokenArgumentsParser")
		I_ASSIGN(m_designTokenFileParserCompPtr, "DesignTokenFileParser", "Design token file parser", true, "IDesignTokenFileParser")
	I_END_COMPONENT;

	// reimplemented (IDesignTokenProcessor)
	virtual int Exec() override;
	virtual QByteArray GetHelpString() const override;


private:
	constexpr static const char* s_suffixOffNormal = "_Off_Normal";
	constexpr static const char* s_suffixOffDisabled = "_Off_Disabled";
	constexpr static const char* s_suffixOffActive = "_Off_Active";
	constexpr static const char* s_suffixOffSelected = "_Off_Selected";
	constexpr static const char* s_suffixOnNormal = "_On_Normal";
	constexpr static const char* s_suffixOnDisabled = "_On_Disabled";
	constexpr static const char* s_suffixOnActive = "_On_Active";
	constexpr static const char* s_suffixOnSelected = "_On_Selected";

	I_REF(IDesignTokenArgumentsParser, m_argumentParserCompPtr);
	I_REF(IDesignTokenFileParser, m_designTokenFileParserCompPtr);

	QByteArrayList m_templateIconColorList;
	QMap<QByteArray/*template color*/,QByteArray> m_normalColor;
	QMap<QByteArray/*template color*/,QByteArray> m_offNormalColor;
	QMap<QByteArray/*template color*/,QByteArray> m_offDisabledColor;
	QMap<QByteArray/*template color*/,QByteArray> m_offActiveColor;
	QMap<QByteArray/*template color*/,QByteArray> m_offSelectedColor;
	QMap<QByteArray/*template color*/,QByteArray> m_onNormalColor;
	QMap<QByteArray/*template color*/,QByteArray> m_onDisabledColor;
	QMap<QByteArray/*template color*/,QByteArray> m_onActiveColor;
	QMap<QByteArray/*template color*/,QByteArray> m_onSelectedColor;

	QString m_inputDirName;
	QString m_inputFileName;
	QString m_outputDirName;
	QString m_projectName;
	QFileInfo m_designTokenFileInfo;

private:
	QByteArray GetFileNameForState(const QString& fileName, IDesignTokenFileParser::IconState iconState) const;

	/**
		sets new color in single image
	*/
	bool SetColor(const QString& fileName, const QString& outputFileName, const QByteArray& replacedColor, const QByteArray& replacebleColor) const;

	/**
		sets all available colors for single images
	*/
	bool SetColorForAllModeState(const QString& fileName, const QString& outputDirName) const;

	/**
		sets all colors for all images in directory
	*/
	bool SetColorAllFilesInDir(const QString& inputDirName, const QString& outputDirName) const;

	/**
		Checks for ignore some files e.g.? if it has suffix
	*/
	bool IgnoreFile(const QFileInfo& fileInfo) const;

};


} // namespace imtdesign

