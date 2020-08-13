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


namespace imtlog
{


class CEventHistoryControllerComp:
			public QObject,
			public ibase::TRuntimeStatusHanderCompWrap<
						ilog::TMessageDelegatorComp<ilog::CLoggerComponentBase>>,
			virtual public ilog::IMessageConsumer
{
	Q_OBJECT
public:
	typedef ibase::TRuntimeStatusHanderCompWrap<
				ilog::TMessageDelegatorComp<ilog::CLoggerComponentBase>> BaseClass;

	I_BEGIN_COMPONENT(CEventHistoryControllerComp)
		I_REGISTER_INTERFACE(ilog::IMessageConsumer);
		I_ASSIGN(m_logFolderCompPtr, "LogFolder", "Path to the event history folder", true, "");
		I_ASSIGN(m_compressorCompPtr, "FileCompressor", "File compressor", false, "");
		I_ASSIGN(m_versionInfoCompPtr, "VersionInfo", "Version info", true, "VersionInfo");
		I_ASSIGN(m_containerTimeDurationAttrPtr, "ContainerTimeRangeDuration", "Container time range duration in seconds", true, 600);
		I_ASSIGN(m_containerWriteDelayAttrPtr, "ContainerWriteDelay", "Delay before sending the message container for writig in seconds", true, 300);
		I_ASSIGN(m_containerNameFormatAttrPtr, "ContainerNameFormat", "Container name format", true, "dd.MM.yyyy hh-mm-ss.zzz");
		I_ASSIGN(m_containerExtensionAttrPtr, "ContainerFileExtension", "Container file extension", true, "xml");
		I_ASSIGN(m_archiveNameFormatAttrPtr, "ArchiveNameFormat", "Archive name format", true, "dd.MM.yyyy");
		I_ASSIGN(m_archiveExtensionAttrPtr, "ArchiveFileExtension", "Archive file extension", true, "arc");
	I_END_COMPONENT;

	CEventHistoryControllerComp();

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


	struct TimeRange
	{
		QDateTime beginTime;
		QDateTime endTime;
	};

	class EventContainer:
				virtual public ilog::IMessageContainer,
				virtual public ilog::IMessageConsumer
	{
	public:
		const QDateTime& GetBeginTime();
		void SetBeginTime(const QDateTime& time);

		const QDateTime& GetEndTime();
		void SetEndTime(const QDateTime& time);

		int GetMessagesCount();

		// reimplemented (ilog::IMessageConsumer)
		virtual bool IsMessageSupported(
					int messageCategory = -1,
					int messageId = -1,
					const istd::IInformationProvider* messagePtr = NULL) const override;
		virtual void AddMessage(const MessagePtr& messagePtr) override;

		// reimplemented (ilog::IMessageContainer)
		virtual int GetWorstCategory() const override;
		virtual Messages GetMessages() const override;
		virtual void ClearMessages() override;

		// reimplemented (iser::ISerializable)
		bool Serialize(iser::IArchive & archive) override;

	private:
		mutable QMutex m_mutex;

		ilog::CMessageContainer m_messageContainer;
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

private Q_SLOTS:
	void OnContainerCheckTimer();

private:
	EventContainerPtr GetContainerForMessage(const MessagePtr& messagePtr);
	TimeRange CalculateContainerTimeRange(const QDateTime& lastContainerEndTime);
	void PrepareWorkingContainers();
	bool SerializeContainer(EventContainerPtr containerPtr, iser::IArchive& archive);
	

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

	I_REF(ifile::IFileNameParam, m_logFolderCompPtr);
	I_REF(imtfile::IFileCompression, m_compressorCompPtr);
	I_REF(iser::IVersionInfo, m_versionInfoCompPtr);
	I_ATTR(int, m_containerTimeDurationAttrPtr);
	I_ATTR(int, m_containerWriteDelayAttrPtr);
	I_ATTR(QString, m_containerNameFormatAttrPtr);
	I_ATTR(QString, m_containerExtensionAttrPtr);
	I_ATTR(QString, m_archiveNameFormatAttrPtr);
	I_ATTR(QString, m_archiveExtensionAttrPtr);
};


} // namespace imtlog


