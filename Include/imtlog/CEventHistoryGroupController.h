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
#include <imtlog/CEventHistoryGroupReadAccessor.h>


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
			virtual public IEventProvider,
			virtual public ITimeRangeProvider
{
	Q_OBJECT
public:
	typedef ilog::CLoggerBase BaseClass;

	explicit CEventHistoryGroupController(
				const EventHistoryGroupControllerParams& controllerParams,
				const EventHistoryGroupPersistenceParams& persistenceParams);

	void OnSystemShutdown();

	// reimplemented (imtlog::ITimeRangeProvider)
	virtual CTimeRange GetTimeRange() const override;

	// reimplemented (imtlog::IEventProvider)
	virtual IEventProvider::EventContainerPtr GetEvents(IEventProvider::EventFilterPtr filterPtr) const override;

	// reimplemented (ilog::IMessageConsumer)
	virtual bool IsMessageSupported(
				int messageCategory = -1,
				int messageId = -1,
				const istd::IInformationProvider* messagePtr = NULL) const override;
	virtual void AddMessage(const MessagePtr& messagePtr) override;

	// reimplemented (ilog::ILoggable)
	virtual void SetLogPtr(ilog::IMessageConsumer* logPtr);

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

	class Reader: public QThread, public CEventHistoryGroupReadAccessor
	{
	public:
		typedef QThread BaseClass;

		explicit Reader(
				QString groupDir,
				QString containerExtension,
				QString archiveExtension,
				iser::IVersionInfo* versionInfoPtr,
				imtfile::IFileCompression* compressorPtr);

		bool Start(const CTimeRange& timeRange);
		CEventHistoryGroupReadAccessor::EventContainerListPtr GetResult();

	protected:
		void start(Priority = InheritPriority);
		void terminate();
		void quit();

	protected:
		// reimplemented (CEventHistoryGroupReadAccessor)
		virtual bool IsInterruptionRequested() const override;

		// reimplemented (QThread)
		virtual void run() override;

	private:
		CTimeRange m_timeRange;
		CEventHistoryGroupReadAccessor::EventContainerListPtr m_containerListPtr;
	};

	typedef istd::TSmartPtr<Reader> ReaderPtr;

	struct Job
	{
		typedef imod::TModelWrap<ilog::CMessageContainer> Container;

		IEventProvider::EventContainerPtr containerPtr;
		IEventProvider::EventFilterPtr filterPtr;
		ReaderPtr readerPtr;
		QString uuid;
	};

	typedef istd::TSmartPtr<Job> JobPtr;

private Q_SLOTS:
	void OnTimer();
	void OnJobFinished();

private:
	CEventHistoryGroupReadAccessor::EventContainerPtr GetContainerForMessage(const QDateTime& timestamp);

	void StartWriter();
	Q_INVOKABLE void OnWriterFinished();

private:
	ControllerState m_state;
	EventHistoryGroupControllerParams m_controllerParams;
	EventHistoryGroupPersistenceParams m_persistenceParams;
	QTimer m_timer;

	mutable CTimeRange m_archiveTimeRange;
	mutable QQueue<EventContainerPtr> m_cache;

	CEventHistoryGroupReadAccessor::EventContainerList m_workingQueue;
	CEventHistoryGroupReadAccessor::EventContainerList m_writingQueue;
	QMutex m_workingQueueMutex;
	QMutex m_writingQueueMutex;

	Writer m_writer;
	ThreadState m_writerState;

	Reader m_syncReader;

	mutable QMap<Reader*, JobPtr> m_jobs;
	mutable QMutex m_jobMutex;
};


} // namespace imtlog


