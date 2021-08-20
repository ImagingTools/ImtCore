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


class CDesignTokenArgumentsParserComp: public ilog::CLoggerComponentBase, public IDesignTokenArgumentsParser
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDesignTokenArgumentsParserComp);
		I_REGISTER_INTERFACE(IDesignTokenArgumentsParser);
	I_END_COMPONENT;

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
	virtual bool IsImageModeRequired() const override;
	virtual bool IsStyleSheetModeRequired() const override;
	virtual QByteArray GetDesignTokenFilePath() const override;
	virtual QByteArray GetInputDirectoryPath() const override;
	virtual QByteArray GetOutputDirectoryPath() const override;


private:
	QMultiMap<AvailableArguments, QByteArray> m_commands;
	QByteArrayList m_arguments;
};


} // namespace imtstyle


