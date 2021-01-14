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
#include <ilog/TLoggerCompWrap.h>
#include <ilog/IMessageContainer.h>
#include <ilog/TMessageDelegatorComp.h>
#include <ibase/TRuntimeStatusHanderCompWrap.h>
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtfile/IFileCompression.h>
#include <imtlog/IEventProvider.h>
#include <imtlog/CEventContainer.h>
#include <imtlog/CEventHistoryGroupReader.h>
#include <imtlog/CEventHistoryReadJobController.h>


namespace imtlog
{


struct EventHistoryGroupControllerParams
{
	quint32 containerDuration;
	quint16 writeDelay;
	quint16 removeDelay;
};


struct EventHistoryGroupPersistenceParams
{
	QString repositoryDir;
	QString groupDir;
	QString containerExtension;
	QString archiveExtension;
	iser::IVersionInfo* versionInfoPtr;
	imtfile::IFileCompression* compressorPtr;
};


class CEventHistoryGroupController:
			public QObject,
			virtual public ilog::CLoggerBase,
			virtual public ilog::IMessageConsumer,
			virtual public ITimeRangeProvider
{
	Q_OBJECT
public:
	typedef ilog::CLoggerBase BaseClass;

	explicit CEventHistoryGroupController(
				const EventHistoryGroupControllerParams& controllerParams,
				const EventHistoryGroupPersistenceParams& persistenceParams);

	void OnSystemShutdown();

	QByteArray RequestEvents(IEventProvider::EventFilterPtr filterPtr) const;

	bool IsValidRequestId(const QByteArray& requestId) const;
	IEventProvider::EventFilterPtr GetFilter(const QByteArray& requestId) const;
	bool PopResult(const QByteArray& requestId, ilog::CMessageContainer& resultEvents) const;

	// reimplemented (imtlog::ITimeRangeProvider)
	virtual CTimeRange GetTimeRange() const override;

	// reimplemented (ilog::IMessageConsumer)
	virtual bool IsMessageSupported(
				int messageCategory = -1,
				int messageId = -1,
				const istd::IInformationProvider* messagePtr = NULL) const override;
	virtual void AddMessage(const MessagePtr& messagePtr) override;

	// reimplemented (ilog::ILoggable)
	virtual void SetLogPtr(ilog::IMessageConsumer* logPtr);

Q_SIGNALS:
	void RequestFinished(QByteArray requestId);

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

	class Writer: public QThread
	{
	public:
		explicit Writer(CEventHistoryGroupController* parentPtr);

		void SetParams(const EventHistoryGroupPersistenceParams& params);

	private:
		// reimplemented (QThread)
		virtual void run() override;

	private:
		CEventHistoryGroupController* m_parentPtr;
		EventHistoryGroupPersistenceParams m_params;
	};

private Q_SLOTS:
	void OnTimer();
	void OnJobFinished(const QByteArray& jobId);

private:
	CEventHistoryGroupReader::EventContainerPtr GetContainerForMessage(const QDateTime& timestamp);

	void StartWriter();
	Q_INVOKABLE void OnWriterFinished();

private:
	ControllerState m_state;
	EventHistoryGroupControllerParams m_controllerParams;
	EventHistoryGroupPersistenceParams m_persistenceParams;
	QTimer m_timer;

	mutable CTimeRange m_archiveTimeRange;
	mutable QQueue<IEventProvider::EventContainerPtr> m_cache;

	CEventHistoryGroupReader::EventContainerList m_workingQueue;
	CEventHistoryGroupReader::EventContainerList m_writingQueue;
	QMutex m_workingQueueMutex;
	QMutex m_writingQueueMutex;

	Writer m_writer;
	ThreadState m_writerState;

	mutable CEventHistoryReadJobController m_readJobController;
	CEventHistoryGroupReader m_completeGroupReader;
};


} // namespace imtlog


