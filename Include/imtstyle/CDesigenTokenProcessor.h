#pragma once

// Stl includes
#include <iostream>

// Qt includes
#include <QtCore/QtCore>

class CDesigenTokenProcessor
{
public:
	enum class AvailableCommands: int
	{
		HELP				= 0x0001,	///< Display help message
		REPLACEBLE_COLOR	= 0x0002,	///< sets \c m_replaceableColor
		NORMAL_COLOR		= 0x0004,
		OFF_NORMAL_COLOR	= 0x0008,
		OFF_DISABLED_COLOR	= 0x0010,
		OFF_ACTIVE_COLOR	= 0x0020,
		OFF_SELECTED_COLOR	= 0x0040,
		ON_NORMAL_COLOR		= 0x0080,
		ON_DISABLED_COLOR	= 0x0100,
		ON_ACTIVE_COLOR		= 0x0200,
		ON_SELECTED_COLOR	= 0x0400
	};
	CDesigenTokenProcessor(int argc, char **argv, char** env = nullptr);

	int Exec();

private:

	constexpr static const char* SUFFIX_OFF_NORMAL   = "_Off_Normal";
	constexpr static const char* SUFFIX_OFF_DISABLED = "_Off_Disabled";
	constexpr static const char* SUFFIX_OFF_ACTIVE   = "_Off_Active";
	constexpr static const char* SUFFIX_OFF_SELECTED = "_Off_Selected";
	constexpr static const char* SUFFIX_ON_NORMAL    = "_On_Normal";
	constexpr static const char* SUFFIX_ON_DISABLED  = "_On_Disabled";
	constexpr static const char* SUFFIX_ON_ACTIVE    = "_On_Active";
	constexpr static const char* SUFFIX_ON_SELECTED  = "_On_Selected";

	QMultiMap<AvailableCommands, QByteArray> m_commands;
	QByteArrayList m_arguments;

	/// The color that should be replaced by \c m_replacedColor
	QByteArray m_replaceableColor;

	QByteArray m_normalColor;
	QByteArray m_offNormalColor;
	QByteArray m_offDisabledColor;
	QByteArray m_offActiveColor;
	QByteArray m_offSelectedColor;
	QByteArray m_onNormalColor;
	QByteArray m_onDisabledColor;
	QByteArray m_onActiveColor;
	QByteArray m_onSelectedColor;


	//=============================================================Working methods i.e. workers
	int ShowHelp() const;

	void SetColor(const QByteArray& fileName, const QByteArray& outputFileName, const QByteArray& replacedColor) const;
	void SetColorForAllModeState(const QByteArray& fileName, const QByteArray& outputDirName) const;

	void SetColorAllFilesInDir(const QByteArray& inputDirName, const QByteArray& outputDirName) const;




	//=============================================================Private assistants
	/// Checking arguments and returns containing value \c AC
	bool CheckArgument(AvailableCommands AC) const;

	/// Getting argument's key and returns it
	QByteArray GetArgumentKey(AvailableCommands AC) const;

	/// Getting argument's value and returns it
	QByteArray GetArgumentValue(AvailableCommands AC) const;

	/// Getting argument's value and returns it
	QByteArrayList GetArgumentValueMulti(AvailableCommands AC) const;

};

