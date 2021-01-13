#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtCore/QDateTime>
#include <QtCore/QString>


// ACF includes
#include <ilog/CLoggerBase.h>
#include <istd/TSmartPtr.h>

// ImtCore includes
#include <imtfile/IFileCompression.h>
#include <imtlog/IEventProvider.h>
#include <imtlog/CEventContainer.h>


namespace imtlog
{


class CEventHistoryGroupReadAccessor: public ilog::CLoggerBase
{

public:
	typedef istd::TSmartPtr<CEventContainer> EventContainerPtr;
	typedef QList<EventContainerPtr> EventContainerList;
	typedef istd::TSmartPtr<EventContainerList> EventContainerListPtr;

	CEventHistoryGroupReadAccessor(
				QString groupDir,
				QString containerExtension,
				QString archiveExtension,
				iser::IVersionInfo* versionInfoPtr,
				imtfile::IFileCompression* compressorPtr);

	CTimeRange GetGroupTimeRange() const;
	EventContainerListPtr GetContainers(const CTimeRange& timeRange) const;

protected:
	virtual bool IsInterruptionRequested() const;

private:
	QMap<QDate, QString> GetDirMap(const QString& dirPath) const;
	QMap<QDateTime, QString> GetFileMap(const QString& dirPath) const;
	EventContainerPtr ImportContainer(const QString& filePath) const;

private:
	QString m_groupDir;
	QString m_containerExtension;
	QString m_archiveExtension;
	iser::IVersionInfo* m_versionInfoPtr;
	imtfile::IFileCompression* m_compressorPtr;
};


} // namespace imtlog


