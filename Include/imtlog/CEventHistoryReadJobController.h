#pragma once


// Qt includes
#include <QtCore/QDebug>
#include <QtCore/QThread>

// ImtCore includes
#include <imtfile/IFileCompression.h>
#include <imtlog/IEventProvider.h>
#include <imtlog/CEventHistoryGroupReader.h>


namespace imtlog
{


class CEventHistoryReadJobController: public QThread
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
		IEventProvider::EventFilterPtr filterPtr;
		IEventProvider::EventContainerPtr eventContainerPtr;
	};

	typedef QList<Job> JobList;

	explicit CEventHistoryReadJobController(
				QString groupDir,
				QString containerExtension,
				QString archiveExtension,
				iser::IVersionInfo* versionInfoPtr,
				imtfile::IFileCompression* compressorPtr);

	QByteArray AddJob(IEventProvider::EventFilterPtr filterPtr);

	bool IsValidJobId(const QByteArray& jobId) const;
	IEventProvider::EventFilterPtr GetFilter(const QByteArray& jobId) const;
	bool PopResult(const QByteArray& jobId, ilog::CMessageContainer& resultEvents);

Q_SIGNALS:
	void JobFinished(QByteArray jobId);

protected:
	// reimplemented (QThread)
	virtual void run() override;

private:
	Job GetJob(const QByteArray& jobId) const;
	void ProcessJob(Job& job);

private:
	JobList m_jobList;
	mutable QMutex m_jobMutex;

	const QString m_groupDir;
	const QString m_containerExtension;
	const QString m_archiveExtension;
	const iser::IVersionInfo* m_versionInfoPtr;
	const imtfile::IFileCompression* m_compressorPtr;
};


} // namespace imtlog


