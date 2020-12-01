#pragma once


// Qt includes
#include <QtCore/QThread>
#include <QtCore/QQueue>
#include <QtCore/QTimer>

// ACF includes
#include <iser/IVersionInfo.h>
#include <ifile/IFileNameParam.h>
#include <istd/TRange.h>
#include <ilog/TLoggerCompWrap.h>
#include <ilog/TMessageDelegatorComp.h>
#include <ibase/TRuntimeStatusHanderCompWrap.h>
#include <ilog/CMessageContainer.h>

// ImtCore includes
#include <imtfile/IFileCompression.h>
#include <imtlog/IEventTimeRangeFilter.h>
#include <imtlog/IEventMessageIdFilter.h>
#include <imtlog/IMessageHistoryConsumer.h>
#include <imtlog/IMessageHistoryContainer.h>
#include <imtlog/ITimeRangeProvider.h>


namespace imtlog
{


class CEventHistoryControllerComp:
			public QObject,
			public ibase::TRuntimeStatusHanderCompWrap<
						ilog::TMessageDelegatorComp<ilog::CLoggerComponentBase>>,
			virtual public ilog::IMessageConsumer,
			virtual public imtlog::IMessageHistoryContainer,
			virtual public imtlog::IEventTimeRangeFilter,
			virtual public imtlog::IEventMessageIdFilter,
			virtual public imtlog::ITimeRangeProvider
{
	Q_OBJECT
public:
	typedef ibase::TRuntimeStatusHanderCompWrap<
				ilog::TMessageDelegatorComp<ilog::CLoggerComponentBase>> BaseClass;

	I_BEGIN_COMPONENT(CEventHistoryControllerComp)
		I_REGISTER_INTERFACE(ilog::IMessageConsumer);
		I_REGISTER_INTERFACE(imtlog::IMessageHistoryContainer);
		I_REGISTER_INTERFACE(imtlog::IEventTimeRangeFilter);
		I_REGISTER_INTERFACE(imtlog::IEventMessageIdFilter);
		I_REGISTER_INTERFACE(imtlog::ITimeRangeProvider);
		I_ASSIGN(m_logFolderCompPtr, "LogFolder", "Path to the event history folder", true, "");
		I_ASSIGN(m_compressorCompPtr, "FileCompressor", "File compressor", false, "");
		I_ASSIGN(m_versionInfoCompPtr, "VersionInfo", "Version info", true, "VersionInfo");
		I_ASSIGN(m_containerTimeDurationAttrPtr, "ContainerTimeRangeDuration", "Container time range duration in seconds", true, 600);
		I_ASSIGN(m_containerWriteDelayAttrPtr, "ContainerWriteDelay", "Delay before sending the message container for writig in seconds", true, 300);
		I_ASSIGN(m_containerNameFormatAttrPtr, "ContainerNameFormat", "Container name format", true, "dd.MM.yyyy hh-mm-ss.zzz");
		I_ASSIGN(m_containerExtensionAttrPtr, "ContainerFileExtension", "Container file extension", true, "xml");
		I_ASSIGN(m_archiveNameFormatAttrPtr, "ArchiveNameFormat", "Archive name format", true, "dd.MM.yyyy");
		I_ASSIGN(m_archiveExtensionAttrPtr, "ArchiveFileExtension", "Archive file extension", true, "arc");
		I_ASSIGN_MULTI_0(m_messageHistoryConsumerCompPtr, "MessageHistoryConsumer", "Message history consumer", false);
	I_END_COMPONENT;

	CEventHistoryControllerComp();

	// reimplemented (imtlog::ITimeRangeProvider)
	virtual CTimeRange GetTimeRange() const override;

	// reimplemented (imtlog::IEventTimeRangeFilter)
	virtual imtlog::CTimeRange GetEventTimeRangeFilter() const override;
	virtual bool SetEventTimeRangeFilter(const imtlog::CTimeRange& timeRange) override;
	virtual void ClearEventTimeRangeFilter() override;

	// reimplemented (imtlog::IEventMessagesIdFilter)
	virtual Mode GetEventMessageIdFilterMode() const override;
	virtual void SetEventMessageIdFilterMode(Mode mode) override;
	virtual QList<int> GetEventMessageIdFilter() const override;
	virtual bool SetEventMessageIdFilter(const QList<int>& messageIdList) override;
	virtual void ClearEventMessageIdFilter() override;

	// reimplemented (imtlog::IMessageHistoryContainer)
	virtual int GetWorstCategory() const override;
	virtual Messages GetMessages() const override;
	virtual void ClearMessages() override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;
	virtual quint32 GetMinimalVersion(int versionId) const override;

	// reimplemented (ilog::IMessageConsumer)
	virtual bool IsMessageSupported(
				int messageCategory = -1,
				int messageId = -1,
				const istd::IInformationProvider* messagePtr = NULL) const override;
	virtual void AddMessage(const MessagePtr& messagePtr) override;

	// reimplemented (ibase::TRuntimeStatusHanderCompWrap)
	virtual void OnSystemShutdown() override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	enum ControllerState
	{
		CS_OK = 0,
		CS_INIT,
		CS_SHUTDOWN,
		CS_FAILED
	};

	enum ThreadState
	{
		/*
			Thread is idle
		*/
		TS_IDLE = 0,

		/*
			Thread is running
		*/
		TS_RUNNING,

		/*
			Thread is running and next run is pending
		*/
		TS_PENDING
	};

	class EventContainer: virtual public imtlog::IMessageHistoryContainer
	{
	public:
		const QDateTime& GetBeginTime();
		void SetBeginTime(const QDateTime& time);

		const QDateTime& GetEndTime();
		void SetEndTime(const QDateTime& time);

		int GetMessagesCount();
		void AddMessage(const MessagePtr& messagePtr, quint64 id);

		// reimplemented (imtlog::IMessageHistoryContainer)
		virtual int GetWorstCategory() const override;
		virtual Messages GetMessages() const override;
		virtual void ClearMessages() override;

		// reimplemented (iser::ISerializable)
		bool Serialize(iser::IArchive & archive) override;

	private:
		mutable QMutex m_mutex;

		ilog::CMessageContainer m_messageContainer;
		QList<quint64> m_messageIds;
		QDateTime m_beginTime;
		QDateTime m_endTime;
	};

	typedef QSharedPointer<EventContainer> EventContainerPtr;
	typedef QQueue<EventContainerPtr> EventContainerQueue;

	class Reader: public QThread
	{
	public:
		explicit Reader(CEventHistoryControllerComp* parentPtr);

	private:
		void ImportFromDirs(const QStringList& dirList);
		void ImportFromFiles(const QStringList& fileList);

		// reimplemented (QThread)
		virtual void run() override;

	private:
		CEventHistoryControllerComp* m_parentPtr;
	};

	class Writer: public QThread
	{
	public:
		explicit Writer(CEventHistoryControllerComp* parentPtr);

	private:
		// reimplemented (QThread)
		virtual void run() override;

	private:
		CEventHistoryControllerComp* m_parentPtr;
	};

	class MessageCache
	{
	public:
		explicit MessageCache(CEventHistoryControllerComp* parentPtr, int cacheDayCount);

		void Init();
		void AddMessage(const MessagePtr& messagePtr, uint64_t id);
		Messages GetMessages(const imtlog::CTimeRange& timeRange) const;

	private:
		CEventHistoryControllerComp* m_parentPtr;

		int m_cacheDayCount;
		mutable QMap<QDate, imtlog::IMessageHistoryContainer::Messages> m_cache;
		mutable QQueue<QDate> m_cacheQueue;
		mutable QMap<QDate, imtlog::IMessageHistoryContainer::Messages> m_2dayCache;
	};

private Q_SLOTS:
	void OnContainerCheckTimer();
	void OnAddMessage(const MessagePtr& messagePtr, uint64_t id);

private:
	EventContainerPtr GetContainerForMessage(const MessagePtr& messagePtr);
	imtlog::CTimeRange CalculateContainerTimeRange(const QDateTime& lastContainerEndTime);
	void PrepareWorkingContainers();
	QList<EventContainerPtr> ImportContainersFromFile(const QString& file) const;
	void Init();


	void StartReader();
	Q_INVOKABLE void OnReaderFinished();
	Q_INVOKABLE void OnContainerReadComplete(EventContainerPtr containerPtr);

	void StartWriter();
	Q_INVOKABLE void OnWriterFinished();

private:
	QDateTime m_systemStartTime;
	ControllerState m_controllerState;

	Reader m_reader;
	Writer m_writer;
	ThreadState m_readerState;
	ThreadState m_writerState;

	QTimer m_containerCheckTimer;

	EventContainerQueue m_workingQueue;
	EventContainerQueue m_writingQueue;
	QMutex m_workingQueueMutex;
	QMutex m_writingQueueMutex;

	CTimeRange m_filterTimeRange;
	IEventMessageIdFilter::Mode m_filterMessageIdMode;
	QList<int> m_filterMessageIdList;
	CTimeRange m_archiveTimeRange;
	uint64_t m_messageId;

	MessageCache m_messageCache;

	I_REF(ifile::IFileNameParam, m_logFolderCompPtr);
	I_REF(imtfile::IFileCompression, m_compressorCompPtr);
	I_REF(iser::IVersionInfo, m_versionInfoCompPtr);
	I_MULTIREF(IMessageHistoryConsumer, m_messageHistoryConsumerCompPtr);
	I_ATTR(int, m_containerTimeDurationAttrPtr);
	I_ATTR(int, m_containerWriteDelayAttrPtr);
	I_ATTR(QString, m_containerNameFormatAttrPtr);
	I_ATTR(QString, m_containerExtensionAttrPtr);
	I_ATTR(QString, m_archiveNameFormatAttrPtr);
	I_ATTR(QString, m_archiveExtensionAttrPtr);
};


} // namespace imtlog


