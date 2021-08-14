#pragma once


// Qt includes
#include <QtCore/QtCore>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IParamsSet.h>


namespace designtokencreator
{

class CDesignTokenArgumentsParser
{
public:
	enum class AvailableCommands: int
	{
		HELP				= 0x0001,
		REPLACEBLE_COLOR	= 0x0002,
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
	CDesignTokenArgumentsParser();
	void SetParams(int argc, char** argv, iprm::IParamsSet* paramSet);

private:
	constexpr static const char* SUFFIX_OFF_NORMAL   = "_Off_Normal";
	constexpr static const char* SUFFIX_OFF_DISABLED = "_Off_Disabled";
	constexpr static const char* SUFFIX_OFF_ACTIVE   = "_Off_Active";
	constexpr static const char* SUFFIX_OFF_SELECTED = "_Off_Selected";
	constexpr static const char* SUFFIX_ON_NORMAL    = "_On_Normal";
	constexpr static const char* SUFFIX_ON_DISABLED  = "_On_Disabled";
	constexpr static const char* SUFFIX_ON_ACTIVE    = "_On_Active";
	constexpr static const char* SUFFIX_ON_SELECTED  = "_On_Selected";

private:
	/// Checking arguments and returns containing value \c AC
	bool CheckArgument(AvailableCommands AC) const;

	/// Getting argument's key and returns it
	QByteArray GetArgumentKey(AvailableCommands AC) const;

	/// Getting argument's value and returns it
	QByteArray GetArgumentValue(AvailableCommands AC) const;

	/// Getting argument's value and returns it
	QByteArrayList GetArgumentValueMulti(AvailableCommands AC) const;

private:
	QMultiMap<AvailableCommands, QByteArray> m_commands;
	QByteArrayList m_arguments;


};


} // namespace designtokencreator
