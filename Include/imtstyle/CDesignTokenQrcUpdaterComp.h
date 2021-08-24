#pragma once


// Qt includes
#include <QtCore/QtCore>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtstyle/IDesignTokenProcessor.h>
#include <imtstyle/IDesignTokenArgumentsParser.h>
#include <imtstyle/IDesignTokenFileParser.h>
#include <imtstyle/IDesignTokenQrcUpdater.h>


namespace imtstyle
{


class CDesignTokenQrcUpdaterComp: public ilog::CLoggerComponentBase, public IDesignTokenQrcUpdater
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDesignTokenQrcUpdaterComp);
	I_REGISTER_INTERFACE(IDesignTokenQrcUpdater);
	I_END_COMPONENT;

	// reimplemented (IDesignTokenQrcUpdater)
	virtual bool CreateQrc(const QString& prefix, const QString& path, const QFileInfoList& files, const QByteArray& outputFileName) const override;
	virtual bool ModifyQrc(const QString& prefix, const QString& path, const QFileInfoList& files, const QByteArray& outputFileName) const override;


private:

};


} // namespace imtstyle

