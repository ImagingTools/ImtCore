#pragma once


// Qt includes
#include <QtCore/QtCore>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtdesign/IDesignTokenArgumentsParser.h>


namespace imtdesign
{


class CDesignTokenArgumentsParserComp: public ilog::CLoggerComponentBase, public IDesignTokenArgumentsParser
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDesignTokenArgumentsParserComp);
		I_REGISTER_INTERFACE(IDesignTokenArgumentsParser);
	I_END_COMPONENT;

	/**
		Checking arguments and returns containing value \c argument
	*/
	bool CheckArgument(AvailableArguments argument) const;

	/**
		Getting argument's key and returns it
	*/
	QByteArray GetArgumentKey(AvailableArguments argument) const;

	/**
		Getting argument's value and returns it
	*/
	QByteArray GetArgumentValue(AvailableArguments argument) const;

	/**
	Getting argument's value and returns it
	*/
	QByteArrayList GetArgumentValueMulti(AvailableArguments argument) const;

	// reimplemented (IDesignTokenArgumentsParser)
	void SetArguments(int argc, char**argv) override;
	bool IsHelpRequested() const override;
	bool IsSplitModeRequired() const override;
	bool IsErrorsIgnoreRequested() const override;
	bool IsCopyDesignTokenFileRequired() const override;
	QByteArray GetDesignTokenFilePath() const override;
	QByteArray GetImagesInputDirectoryPath() const override;
	QByteArray GetStyleSheetsInputDirectoryPath() const override;
	QByteArray GetOutputDirectoryPath() const override;
	QByteArray GetProjectName() const override;


private:
	QMultiMap<AvailableArguments, QByteArray> m_commands;
	QByteArrayList m_arguments;
};


} // namespace imtdesign


