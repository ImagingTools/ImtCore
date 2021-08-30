#pragma once


// Qt includes
#include <QtCore/QtCore>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IParamsSet.h>


namespace imtstyle
{


class IDesignTokenQrcUpdater
{

public:
	virtual bool CreateQrcRecursivly(const QString& inputDirName, const QByteArray& outputFileName) const = 0;
	virtual bool CreateQrcForDirs(const QStringList& inputDirNames, const QByteArray& outputFileName) const = 0;
	virtual bool CreateQrc(const QString& prefix, const QString& path, const QFileInfoList& files, const QByteArray& outputFileName) const = 0;
	virtual bool ModifyQrc(const QString& prefix, const QString& path, const QFileInfoList& files, const QByteArray& outputFileName) const = 0;

};


} // namespace imtstyle


