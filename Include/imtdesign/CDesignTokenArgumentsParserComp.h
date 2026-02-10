// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QtCore>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

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
	virtual void SetArguments(int argc, char**argv) override;
	virtual bool IsHelpRequested() const override;
	virtual bool IsSplitModeRequired() const override;
	virtual bool IsErrorsIgnoreRequested() const override;
	virtual bool IsCopyDesignTokenFileRequired() const override;
	virtual bool IsWebGenerateResourceRequired() const override;
	virtual bool IsCommonGenerateResourceRequired() const override;
	virtual bool IsQmlGenerateRequired() const override;

	virtual QByteArray GetDesignTokenFilePath() const override;
	virtual QByteArrayList GetDesignTokenFileMultiPath() const override;
	virtual QByteArray GetImagesInputDirectoryPath() const override;
	virtual QByteArrayList GetImagesInputDirectoryMultiPath() const override;
	virtual QByteArray GetStyleSheetsInputDirectoryPath() const override;
	virtual QByteArray GetOutputDirectoryPath() const override;
	virtual QByteArray GetOutputFilePath() const override;
	virtual QByteArray GetProjectName() const override;


private:
	QMultiMap<AvailableArguments, QByteArray> m_commands;
	QByteArrayList m_arguments;
};


} // namespace imtdesign


