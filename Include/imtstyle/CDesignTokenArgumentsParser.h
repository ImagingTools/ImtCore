#pragma once


// Qt includes
#include <QtCore/QtCore>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IParamsSet.h>

/// \todo MOVE it to imtstyle
namespace imtstyle
{


class CDesignTokenArgumentsParser
{
public:
	enum AvailableArguments
	{
		/**
			Displays help
		*/
		AA_HELP,

		/**

		*/
		AA_TEMPLATE_ICON_COLOR,
		AA_TEMPLATE_PRIMARY_COLOR,
		AA_TEMPLATE_SECONDARY_COLOR,
		AA_DESIGN_TOKEN_FILE_PATH
	};
	CDesignTokenArgumentsParser(int argc, char** argv);

	/// Checking arguments and returns containing value \c argument
	bool CheckArgument(AvailableArguments argument) const;

	/// Getting argument's key and returns it
	QByteArray GetArgumentKey(AvailableArguments argument) const;

	/// Getting argument's value and returns it
	QByteArray GetArgumentValue(AvailableArguments argument) const;

	/// Getting argument's value and returns it
	QByteArrayList GetArgumentValueMulti(AvailableArguments argument) const;

private:


private:
	QMultiMap<AvailableArguments, QByteArray> m_commands;
	QByteArrayList m_arguments;
};


} // namespace imtstyle


