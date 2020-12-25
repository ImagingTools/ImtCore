#pragma once


// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QThread>
#include <QtCore/QQueue>
#include <QtCore/QTimer>

// ACF includes
#include <iser/IVersionInfo.h>
#include <ifile/IFileNameParam.h>
#include <ilog/TLoggerCompWrap.h>
#include <ilog/TMessageDelegatorComp.h>
#include <ibase/TRuntimeStatusHanderCompWrap.h>

// ImtCore includes
#include <imtfile/IFileCompression.h>
#include <imtlog/IMessageHistoryProvider.h>
#include <imtlog/CMessageHistoryContainer.h>


namespace imtlog
{


class CEventHistoryControllerComp:
			public QObject,
			public ibase::TRuntimeStatusHanderCompWrap<
						ilog::TMessageDelegatorComp<ilog::CLoggerComponentBase>>,
			virtual public ilog::IMessageConsumer,
			virtual public IMessageHistoryProvider,
			virtual public ITimeRangeProvider
{
	Q_OBJECT
public:
	typedef ibase::TRuntimeStatusHanderCompWrap<
				ilog::TMessageDelegatorComp<ilog::CLoggerComponentBase>> BaseClass;

	I_BEGIN_COMPONENT(CEventHistoryControllerComp)
		I_REGISTER_INTERFACE(ilog::IMessageConsumer);
		I_REGISTER_INTERFACE(imtlog::IMessageHistoryProvider);
		I_REGISTER_INTERFACE(imtlog::ITimeRangeProvider);
		I_ASSIGN(m_logFolderCompPtr, "LogFolder", "Path to the event history folder", true, "");
		I_ASSIGN(m_compressorCompPtr, "FileCompressor", "File compressor", false, "");
		I_ASSIGN(m_versionInfoCompPtr, "VersionInfo", "Version info", true, "VersionInfo");
		I_ASSIGN(m_containerTimeDurationAttrPtr, "ContainerTimeRangeDuration", "Container time range duration in seconds", true, 600);
		I_ASSIGN(m_containerWriteDelayAttrPtr, "ContainerWriteDelay", "Delay before sending the message container for writig in seconds", true, 300);
		I_ASSIGN(m_containerExtensionAttrPtr, "ContainerFileExtension", "Container file extension", true, "xml");
		I_ASSIGN(m_archiveExtensionAttrPtr, "ArchiveFileExtension", "Archive file extension", true, "arc");
		I_ASSIGN_MULTI_0(m_messageHistoryConsumerCompPtr, "MessageHistoryConsumer", "Message history consumer", false);
	I_END_COMPONENT;

	CEventHistoryControllerComp();

	// reimplemented (imtlog::ITimeRangeProvider)
	virtual CTimeRange GetTimeRange() const override;

	// reimplemented (imtlog::IMessageHistoryProvider)
	virtual IMessageHistoryConsumer::Messages GetMessages(const IMessageFilter* filterPtr) const override;

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

	typedef istd::TSmartPtr<CMessageHistoryContainer> MessageHistoryContainerPtr;
	typedef QQueue<MessageHistoryContainerPtr> EventContainerQueue;

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

private Q_SLOTS:
	void OnContainerCheckTimer();
	void OnAddMessage(const MessagePtr& messagePtr, quint64 id);

private:
	MessageHistoryContainerPtr GetContainerForMessage(const MessagePtr& messagePtr);
	imtlog::CTimeRange CalculateContainerTimeRange(const QDateTime& timestamp);
	QList<MessageHistoryContainerPtr> ImportContainersFromFile(const QString& file) const;
	void Init();

	QMap<QDate, QString> GetHistoryDirMap() const;
	QMap<QDateTime, QString> GetHistoryFileMap(const QString& dir) const;
	MessageHistoryContainerPtr LoadHistoryContainer(const QString& filePath) const;

	void StartWriter();
	Q_INVOKABLE void OnWriterFinished();

private:
	QDateTime m_systemStartTime;
	ControllerState m_controllerState;

	Writer m_writer;
	ThreadState m_readerState;
	ThreadState m_writerState;

	QTimer m_containerCheckTimer;

	EventContainerQueue m_workingQueue;
	EventContainerQueue m_writingQueue;
	QMutex m_workingQueueMutex;
	QMutex m_writingQueueMutex;

	CTimeRange m_archiveTimeRange;
	uint64_t m_messageId;

	mutable QList<MessageHistoryContainerPtr> m_cache;

	I_REF(ifile::IFileNameParam, m_logFolderCompPtr);
	I_REF(imtfile::IFileCompression, m_compressorCompPtr);
	I_REF(iser::IVersionInfo, m_versionInfoCompPtr);
	I_MULTIREF(IMessageHistoryConsumer, m_messageHistoryConsumerCompPtr);
	I_ATTR(int, m_containerTimeDurationAttrPtr);
	I_ATTR(int, m_containerWriteDelayAttrPtr);
	I_ATTR(QString, m_containerExtensionAttrPtr);
	I_ATTR(QString, m_archiveExtensionAttrPtr);
};


} // namespace imtlog


