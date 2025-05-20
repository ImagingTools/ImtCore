#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QReadWriteLock>
#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QFileSystemWatcher>

// ACF includes
#include <iprm/IParamsSet.h>
#include <iprm/IOptionsList.h>
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFileNameParam.h>

// Acula includes
#include <imthype/IJobQueueManager.h>
#include <imthype/CStandardJobOutput.h>


namespace imthype
{


class CJobQueueManagerCompBase:
			public QObject,
			public ilog::CLoggerComponentBase,
			virtual public IJobQueueManager
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CJobQueueManagerCompBase);
		I_REGISTER_INTERFACE(IJobQueueManager);
		I_ASSIGN(m_dataFolderCompPtr, "DataFolder", "Path used for persistence of the job data", true, "DataFolder");
		I_ASSIGN(m_versionInfoCompPtr, "VersionInfo", "Version info used for persistence of the job items", true, "VersionInfo");
		I_ASSIGN(m_pollFileSystemAttrPtr, "PollFileSystem", "If enabled, the folder with the job items will be observed and the items will be re-read on changes in the folder structure", true, false);
	I_END_COMPONENT;

	CJobQueueManagerCompBase();

	// reimplemented (IJobQueueManager)
	virtual QByteArray GetTaskTypeId(const QByteArray& jobId) const override;
	virtual QByteArray InsertNewJobIntoQueue(
				const QByteArray& contextId,
				const QByteArray& typeId,
				const imtbase::IReferenceCollection& input,
				const iprm::IParamsSet* jobProcessingParamsPtr,
				const IJobSchedulerParams* schedulerParamsPtr = nullptr,
				ilog::IMessageConsumer* logPtr = nullptr) override;
	virtual bool CancelJob(const QByteArray& jobId) override;
	virtual bool ResumeJob(const QByteArray& jobId) override;
	virtual bool RemoveJob(const QByteArray& jobId) override;
	virtual bool GetJobConfiguration(
				const QByteArray& jobId,
				ParamsPtr& processingParamsPtr,
				imtbase::IReferenceCollection& input) const override;
	virtual ProcessingStatus GetProcessingStatus(const QByteArray& jobId) const override;
	virtual bool SetProcessingStatus(const QByteArray& jobId, ProcessingStatus status) override;
	virtual double GetProgress(const QByteArray& jobId) const override;
	virtual bool SetProgress(const QByteArray & jobId, double progress) override;
	virtual bool GetJobResult(const QByteArray& jobId, IJobOutput& result) const override;
	virtual bool SetJobResult(const QByteArray & jobId, const IJobOutput & result) override;

	// reimplemented (imtbase::ICollectionInfo)
	virtual int GetElementsCount(
				const iprm::IParamsSet* selectionParamPtr = nullptr,
				ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual Ids GetElementIds(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual bool GetSubsetInfo(
				imtbase::ICollectionInfo& subsetInfo,
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* selectionParamsPtr = nullptr,
				ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual QVariant GetElementInfo(const QByteArray& elementId, int infoType, ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual idoc::MetaInfoPtr GetElementMetaInfo(const Id& elementId, ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual bool SetElementName(const Id& elementId, const QString& name, ilog::IMessageConsumer* logPtr = nullptr) override;
	virtual bool SetElementDescription(const Id& elementId, const QString& description, ilog::IMessageConsumer* logPtr = nullptr) override;
	virtual bool SetElementEnabled(const Id& elementId, bool isEnabled = true, ilog::IMessageConsumer* logPtr = nullptr) override;

protected:
	struct JobItem
	{
		JobItem()
			:processingStatus(PS_NONE),
			progress(0.0)
		{
		}

		QByteArray typeId;
		QByteArray uuid;
		QString name;
		QByteArray contextId;
		istd::TSmartPtr<iprm::IParamsSet> paramsPtr;
		ProcessingStatus processingStatus;
		double progress;

		CStandardJobOutput results;
		imtbase::CSimpleReferenceCollection input;
	};

	typedef QVector<JobItem> JobItems;

protected:
	virtual bool SerializeJobItem(JobItem& item, iser::IArchive& archive) const;
	virtual QString SaveJobItem(const JobItem& jobItem) const;
	virtual QString GetJobItemPath(const QByteArray& jobId) const;

	int FindItemById(const QByteArray& itemId) const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	void ReadJobItems(JobItems& items) const;

private Q_SLOTS:
	void OnSync();

protected:
	I_REF(ifile::IFileNameParam, m_dataFolderCompPtr);
	I_REF(iser::IVersionInfo, m_versionInfoCompPtr);
	I_ATTR(bool, m_pollFileSystemAttrPtr);

	JobItems m_jobItems;

	bool m_directoryBlocked;

	class DirectoryBlocker
	{
	public:
		DirectoryBlocker(CJobQueueManagerCompBase& parent)
			:m_parent(parent)
		{
			parent.m_directoryBlocked = true;
		}

		~DirectoryBlocker()
		{
			m_parent.m_directoryBlocked = false;
		}

	private:
		CJobQueueManagerCompBase& m_parent;
	};

	mutable QReadWriteLock m_mutex;

	QTimer m_syncTimer;
};


} // namespace imthype


