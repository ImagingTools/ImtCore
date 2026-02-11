// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QReadWriteLock>
#include <QtCore/QObject>
#include <QtCore/QHash>

// ACF includes
#include <iprm/IParamsSet.h>
#include <iprm/IOptionsList.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>

// Acula includes
#include <imthype/IJobQueueManager.h>
#include <imthype/IJobTicket.h>
#include <imthype/IJobStatus.h>
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
		I_ASSIGN(m_jobTicketsCollectionCompPtr, "JobTicketsCollection", "External object collection containing job tickets", true, "JobTicketsCollection");
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
	IJobTicketSharedPtr GetJobTicket(const QByteArray& jobId) const;
	IJobStatusSharedPtr GetJobStatus(const QByteArray& jobId) const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

protected:
	I_REF(imtbase::IObjectCollection, m_jobTicketsCollectionCompPtr);

	mutable QReadWriteLock m_mutex;
	
	// Separate storage for job status and progress
	// This allows lightweight status queries without loading full job tickets
	mutable QHash<QByteArray, IJobStatusSharedPtr> m_jobStatusMap;
};


} // namespace imthype


