#pragma once


// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QThread>
#include <QtCore/QQueue>
#include <QtCore/QTimer>
#include <QtCore/QSignalMapper>

// ACF includes
#include <iser/IVersionInfo.h>
#include <ifile/IFileNameParam.h>
#include <ilog/IMessageContainer.h>
#include <ibase/TRuntimeStatusHanderCompWrap.h>
#include <ilog/TLoggerCompWrap.h>
#include <ilog/TMessageDelegatorComp.h>
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtfile/IFileCompression.h>
#include <imtlog/IEventProvider.h>
#include <imtlog/CEventContainer.h>
#include <imtlog/CMessagesReader.h>
#include <imtlog/CMessagesReadJobController.h>
#include <imtlog/CWorkerBase.h>


namespace imtlog
{


class CMessagesController:
			public QObject,
			virtual public ilog::CLoggerBase,
			virtual public ilog::IMessageConsumer,
			virtual public ITimeRangeProvider
{
	Q_OBJECT
public:
	typedef ilog::CLoggerBase BaseClass;

	CMessagesController(
				int id,
				quint32 maxContainerMessageCount,
				quint16 containerSavingPeriod,
				quint16 containerCachingTime,
				QString dir,
				QString containerExtension,
				QString archiveExtension,
				iser::IVersionInfo* versionInfoPtr,
				imtfile::IFileCompression* compressorPtr,
				ilog::IMessageConsumer* logPtr);

	void OnSystemShutdown();

	QByteArray AddJob(
				const IEventFilter* filterPtr,
				const IMessageFilterParams* filterParamsPtr) const;

	bool IsJobExists(const QByteArray& jobId) const;
	bool TakeJobResult(const QByteArray& jobId, CMessagesReader::EventContainerPtr& resultEvents) const;

	// reimplemented (imtlog::ITimeRangeProvider)
	virtual CTimeRange GetTimeRange() const override;

	// reimplemented (ilog::IMessageConsumer)
	virtual bool IsMessageSupported(
				int messageCategory = -1,
				int messageId = -1,
				const istd::IInformationProvider* messagePtr = NULL) const override;
	virtual void AddMessage(const MessagePtr& messagePtr) override;

Q_SIGNALS:
	void InitTimer();
	void ReadFinished(QByteArray requestId);

private Q_SLOTS:
	void OnInitTimer();
	void OnTimer();
	void OnJobFinished(const QByteArray& jobId);

private:
	enum ControllerState
	{
		CS_INIT = 0,
		CS_OK,
		CS_SHUTDOWN,
		CS_FAILED
	};

	enum ThreadState
	{
		TS_IDLE = 0,
		TS_RUNNING,
		TS_PENDING
	};

	struct QueueItem
	{
		quint16 timeout;
		CMessagesReader::EventContainerPtr containerPtr;
	};

	class Writer: public CWorkerBase
	{
	public:
		Writer(
					CMessagesController& parent,
					QString dir,
					QString containerExtension,
					QString archiveExtension,
					iser::IVersionInfo* versionInfoPtr,
					imtfile::IFileCompression* compressorPtr);

	private:
		// reimplemented (QThread)
		virtual void run() override;

	private:
		CMessagesController& m_parent;
		QString m_dir;
		QString m_containerExtension;
		QString m_archiveExtension;
		iser::IVersionInfo* m_versionInfoPtr;
		imtfile::IFileCompression* m_compressorPtr;
	};

private:
	ControllerState m_state;

	int m_maxContainerMessageCount;
	quint16 m_containerSavingPeriod;
	quint16 m_containerCachingTime;

	QTimer m_timer;
	Writer m_writer;

	mutable CTimeRange m_archiveTimeRange;

	QList<QueueItem> m_workingQueue;
	QMutex m_workingQueueMutex;

	CMessagesReader::EventContainerList m_writingQueue;
	QMutex m_writingQueueMutex;

	QList<QueueItem> m_cache;
	QMutex m_cacheMutex;

	mutable CMessagesReadJobController m_readJobController;
	CMessagesReader m_messagesReader;
	int m_supportedMessageId;
};


} // namespace imtlog


