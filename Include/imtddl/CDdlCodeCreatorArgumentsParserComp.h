#pragma once


// Qt includes
#include <QtCore/QtCore>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtddl/IDdlCodeCreatorArgumentsParser.h>


namespace imtddl
{


class CDdlCodeCreatorArgumentsParserComp: public ilog::CLoggerComponentBase, public IDdlCodeCreatorArgumentsParser
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDdlCodeCreatorArgumentsParserComp);
		I_REGISTER_INTERFACE(IDdlCodeCreatorArgumentsParser);
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


	// reimplemented (IDdlCodeCreatorArgumentsParser)
	virtual void SetArguments(int argc, char**argv) override;
	virtual QByteArray GetTemplateDirPath() const override;
	virtual QByteArray GetCppDirPath() const override;
	virtual QByteArray GetQmlDirPath() const override;
	virtual QString GetModuleCppName() const override;
	virtual QString GetModuleQmlName() const override;

private:
	QMultiMap<AvailableArguments, QByteArray> m_commands;
	QByteArrayList m_arguments;
};


} // namespace imtddl


