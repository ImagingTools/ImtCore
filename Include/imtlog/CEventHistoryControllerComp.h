#pragma once


// Qt includes
#include <QtCore/QThread>

// ACF includes
#include <ifile/IFileNameParam.h>
#include <ilog/CMessageContainer.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtfile/IFileCompression.h>


namespace imtlog
{


class CEventHistoryControllerComp:
			public QObject,
			public ilog::CLoggerComponentBase,
			virtual public ilog::IMessageConsumer
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CEventHistoryControllerComp)
		I_REGISTER_INTERFACE(ilog::IMessageConsumer);
		I_ASSIGN(m_repositoryPathCompPtr, "RepositoryPath", "Path to the event history folder", true, "");
		I_ASSIGN(m_compressorCompPtr, "FileCompressor", "File compressor", false, "");
		I_ASSIGN(m_slaveMessageConsumerCompPtr, "SlaveMessageConsumer", "Slave message consumer", false, "");
		I_ASSIGN(m_flushingIntervalAttrPtr, "FlushingInterval", "Flushing interval for history in seconds", true, 5);
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

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

protected:
	// reimplemented (QObject)
	virtual void timerEvent(QTimerEvent *event) override;

private:
	struct IntervalContainer
	{
		ilog::CMessageContainer messageContainer;
		QDateTime beginTime;
		QDateTime endTime;
	};
	typedef QSharedPointer<IntervalContainer> IntervalContainerPtr;
	typedef QList<IntervalContainerPtr> IntervalContainerQueue;

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

	class Reader: public QThread
	{
	public:
		explicit Reader(CEventHistoryControllerComp* parentPtr);

	private:
		void DisableHistory();
		void ImportFromDirs(const QStringList& dirList);
		void ImportFromFiles(const QStringList& fileList);

		// reimplemented (QThread)
		virtual void run() override;

	private:
		CEventHistoryControllerComp* m_parentPtr;
		bool m_isInterrupted;
	};

	class Writer: public QThread
	{
	public:
		explicit Writer(CEventHistoryControllerComp* parentPtr);

	private:
		void DisableHistory();

		// reimplemented (QThread)
		virtual void run() override;

	private:
		CEventHistoryControllerComp* m_parentPtr;
		bool m_isInterrupted;
	};

private:
	void StartDayTimer();
	void StartIntervalTimer();
	bool IsCurrentInterval(const QDateTime& timestamp);
	bool IsCurrentDay(const QDateTime& timestamp);
	void CloseInterval();
	bool SerializeContainer(IntervalContainerPtr intervalContainerPtr, iser::IArchive& archive);

	Q_INVOKABLE void AddMessageSync(const MessagePtr messagePtr);
	Q_INVOKABLE void DisableHistory();

	void StartReader();
	Q_INVOKABLE void OnReaderFinished();
	Q_INVOKABLE void OnReaderInterrupted();
	Q_INVOKABLE void OnContainerReadComplete(IntervalContainerPtr intervalContainerPtr);

	void StartWriter();
	Q_INVOKABLE void OnWriterFinished();
	Q_INVOKABLE void OnWriterInterrupted();

private:
	QDateTime m_startedAt;
	bool m_isHistoryActive;

	Reader m_reader;
	Writer m_writer;
	ThreadState m_readerState;
	ThreadState m_writerState;

	QMutex m_queueMutex;

	int m_idDayTimer;
	int m_idIntervalTimer;

	IntervalContainerPtr m_containerPtr;
	IntervalContainerQueue m_containerQueue;

	I_REF(ifile::IFileNameParam, m_repositoryPathCompPtr);
	I_REF(imtfile::IFileCompression, m_compressorCompPtr);
	I_REF(ilog::IMessageConsumer, m_slaveMessageConsumerCompPtr);
	I_ATTR(int, m_flushingIntervalAttrPtr);
	I_ATTR(QString, m_containerNameFormatAttrPtr);
	I_ATTR(QString, m_containerExtensionAttrPtr);
	I_ATTR(QString, m_archiveNameFormatAttrPtr);
	I_ATTR(QString, m_archiveExtensionAttrPtr);
};


} // namespace imtlog


