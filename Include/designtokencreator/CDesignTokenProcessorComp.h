#pragma once


// Qt includes
#include <QtCore/QtCore>

// ACF includes
#include <ilog/TLoggerCompWrap.h>


namespace designtokencreator
{

class CDesignTokenProcessorComp: public ilog::CLoggerComponentBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDesignTokenProcessorComp);

	I_END_COMPONENT;

	int Exec();


protected:
	// reimplemented (ilog::CLoggerComponentBase)
	virtual void OnComponentCreated() override;

private:
	constexpr static const char* SUFFIX_OFF_NORMAL   = "_Off_Normal";
	constexpr static const char* SUFFIX_OFF_DISABLED = "_Off_Disabled";
	constexpr static const char* SUFFIX_OFF_ACTIVE   = "_Off_Active";
	constexpr static const char* SUFFIX_OFF_SELECTED = "_Off_Selected";
	constexpr static const char* SUFFIX_ON_NORMAL    = "_On_Normal";
	constexpr static const char* SUFFIX_ON_DISABLED  = "_On_Disabled";
	constexpr static const char* SUFFIX_ON_ACTIVE    = "_On_Active";
	constexpr static const char* SUFFIX_ON_SELECTED  = "_On_Selected";

	/// The color that should be replaced by \c m_replacedColor
	QByteArray m_replaceableColor;

	/// color for normal image
	QByteArray m_normalColor;
	/// color for _Off_Normal image
	QByteArray m_offNormalColor;
	/// color for _Off_Disabled image
	QByteArray m_offDisabledColor;
	/// color for _Off_Active image
	QByteArray m_offActiveColor;
	/// color for _Off_Selected image
	QByteArray m_offSelectedColor;
	/// color for _On_Normal image
	QByteArray m_onNormalColor;
	/// color for _On_Disabled image
	QByteArray m_onDisabledColor;
	/// color for _On_Active image
	QByteArray m_onActiveColor;
	/// color for _On_Selected image
	QByteArray m_onSelectedColor;

	QByteArray m_inputDirName;
	QByteArray m_inputFileName;
	QByteArray m_outputDirName;


private:
	///displays help
	int ShowHelp() const;
	/// sets new color in single image
	void SetColor(const QByteArray& fileName, const QByteArray& outputFileName, const QByteArray& replacedColor) const;
	/// sets all available colors for single images
	void SetColorForAllModeState(const QByteArray& fileName, const QByteArray& outputDirName) const;
	/// sets all colors for all images in directory
	void SetColorAllFilesInDir(const QByteArray& inputDirName, const QByteArray& outputDirName) const;

};


} // namespace designtokencreator
