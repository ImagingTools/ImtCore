#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QDateTime>
#include <QtCore/QString>


// ACF includes
#include <ilog/IMessageConsumer.h>
#include <istd/TSmartPtr.h>

// ImtCore includes
#include <imtfile/IFileCompression.h>
#include <imtlog/IEventProvider.h>
#include <imtlog/CEventContainer.h>


namespace imtlog
{


class CMessagesReader: virtual public istd::IPolymorphic
{
public:
	typedef istd::TSmartPtr<CEventContainer> EventContainerPtr;
	typedef QList<EventContainerPtr> EventContainerList;
	typedef istd::TSmartPtr<EventContainerList> EventContainerListPtr;

	CMessagesReader(
				const QString& dir,
				const QString& containerExtension,
				const QString& archiveExtension,
				const iser::IVersionInfo* versionInfoPtr,
				const imtfile::IFileCompression* compressorPtr,
				ilog::IMessageConsumer* logPtr);

	CTimeRange ReadTimeRange() const;
	EventContainerListPtr ReadContainers(const CTimeRange& timeRange) const;
	virtual void Cancel();

private:
	void SendErrorMessage(const QString& message) const;

private:
	QMap<QDate, QString> GetDirMap(const QString& dirPath) const;
	QMap<QDateTime, QString> GetFileMap(const QString& dirPath) const;
	EventContainerPtr ImportContainer(const QString& filePath) const;

private:
	const QString m_dir;
	const QString m_containerExtension;
	const QString m_archiveExtension;
	const iser::IVersionInfo* m_versionInfoPtr;
	const imtfile::IFileCompression* m_compressorPtr;
	bool m_isCanceled;

	ilog::IMessageConsumer* m_logPtr;
};


} // namespace imtlog


