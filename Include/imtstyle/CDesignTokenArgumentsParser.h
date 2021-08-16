#pragma once


// Qt includes
#include <QtCore/QtCore>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtstyle/IDesignTokenArgumentsParser.h>


namespace imtstyle
{


class CDesignTokenArgumentsParser: IDesignTokenArgumentsParser
{
public:

	/// Checking arguments and returns containing value \c argument
	bool CheckArgument(AvailableArguments argument) const;

	/// Getting argument's key and returns it
	QByteArray GetArgumentKey(AvailableArguments argument) const;

	/// Getting argument's value and returns it
	QByteArray GetArgumentValue(AvailableArguments argument) const;

	/// Getting argument's value and returns it
	QByteArrayList GetArgumentValueMulti(AvailableArguments argument) const;

	// reimplemented (IDesignTokenArgumentsParser)
	virtual void SetArguments(int argc, char** argv) override;
	virtual bool IsHelpRequested() const override;
	virtual QByteArray GetDesignTokenFilePath() const override;
	virtual QByteArray GetTemplateIconColor() const override;
	virtual QByteArray GetTemplatePrimaryColor() const override;
	virtual QByteArray GetTemplateSecondaryColor() const override;

private:


private:
	QMultiMap<AvailableArguments, QByteArray> m_commands;
	QByteArrayList m_arguments;
};


} // namespace imtstyle


