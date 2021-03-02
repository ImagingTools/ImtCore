#pragma once


// Qt includes
#include <QtCore/QThread>

// ACF includes
#include <ilog/IMessageConsumer.h>
#include <ilog/CMessageContainer.h>

// ImtCore includes
#include <imtfile/IFileCompression.h>
#include <imtlog/IEventProvider.h>
#include <imtlog/CMessagesReader.h>
#include <imtlog/CMessageFilterParams.h>


namespace imtlog
{


class CMessagesReadJobController: public QThread
{
	Q_OBJECT
public:
	typedef QThread BaseClass;

	enum JobStatus
	{
		JS_NONE,
		JS_WAITING,
		JS_RUNNING,
		JS_FINISHED
	};

	struct Job
	{
		Job()
			:status(JS_NONE)
		{
		}

		JobStatus status;
		QByteArray uuid;
		const imtlog::IEventFilter* filterPtr;
		imtlog::CMessageFilterParams filterParams;
		CMessagesReader::EventContainerPtr containerPtr;
	};

	typedef QList<Job> JobList;

	explicit CMessagesReadJobController(
				QString dir,
				QString containerExtension,
				QString archiveExtension,
				iser::IVersionInfo* versionInfoPtr,
				imtfile::IFileCompression* compressorPtr,
				ilog::IMessageConsumer* logPtr);

	QByteArray AddJob(const IEventFilter* filterPtr, const IMessageFilterParams* filterParamsPtr);

	bool IsJobExists(const QByteArray& jobId) const;
	bool TakeJobResult(const QByteArray& jobId, CMessagesReader::EventContainerPtr& resultEvents);

Q_SIGNALS:
	void JobFinished(QByteArray jobId);

protected:
	// reimplemented (QThread)
	virtual void run() override;

private:
	void ProcessJob(Job& job);

private:
	JobList m_jobList;
	mutable QMutex m_jobMutex;

	const QString m_dir;
	const QString m_containerExtension;
	const QString m_archiveExtension;
	const iser::IVersionInfo* m_versionInfoPtr;
	const imtfile::IFileCompression* m_compressorPtr;

	ilog::IMessageConsumer* m_logPtr;
};


} // namespace imtlog


