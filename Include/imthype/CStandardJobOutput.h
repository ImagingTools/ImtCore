// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtbase/CSimpleReferenceCollection.h>

// Acula includes
#include <imthype/IJobOutput.h>
#include <imthype/CProcessingInfo.h>
#include <imthype/CJobExecutionLog.h>

// ACF includes
#include <ilog/CMessageContainer.h>


namespace imthype
{


class CStandardJobOutput: virtual public IJobOutput
{
public:
	typedef CProcessingInfo BaseClass;
	typedef imtbase::CSimpleReferenceCollection BaseClass2;

	/**
		Set job name.
	*/
	void SetJobName(const QString& jobName);

	/**
		Set job's start time.
	*/
	void SetStartTime(const QDateTime& startTime);

	/**
		Set job's finish time.
	*/
	void SetFinishedTime(const QDateTime& finishedTime);

	/**
		Set result category of the job processing.
	*/
	void SetResultCategory(istd::IInformationProvider::InformationCategory resultCategory);


	// reimplemented (IProcessingInfo)
	ProcessingInfo GetProcessingInfo() const override;
	const ilog::IMessageContainer& GetProcessingLog() const override;

	// reimplemented (imtbase::IReferenceCollection)
	virtual ICollectionObjectLinkPtr GetObjectLink(const Id& objectId, const QByteArray& repositoryId = QByteArray()) const override;
	bool InsertReference(const Id& objectId, const QByteArray& repositoryId = QByteArray()) override;
	bool RemoveReference(const Id& objectId) override;
	bool RemoveAllReferences() override;

	// reimplemented (imtbase::IObjectCollectionInfo)
	virtual const iprm::IOptionsList* GetObjectTypesInfo() const override;
	virtual QByteArray GetObjectTypeId(const Id& objectId) const override;
	virtual idoc::MetaInfoPtr GetDataMetaInfo(const Id& objectId) const override;

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
		ICollectionInfo& subsetInfo,
		int offset = 0,
		int count = -1,
		const iprm::IParamsSet* selectionParamsPtr = nullptr,
		ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual QVariant GetElementInfo(const Id& elementId, int infoType, ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual idoc::MetaInfoPtr GetElementMetaInfo(const Id& elementId, ilog::IMessageConsumer* logPtr = nullptr) const override;
	virtual bool SetElementName(const Id& elementId, const QString& name, ilog::IMessageConsumer* logPtr = nullptr) override;
	virtual bool SetElementDescription(const Id& elementId, const QString& description, ilog::IMessageConsumer* logPtr = nullptr) override;
	virtual bool SetElementEnabled(const Id& elementId, bool isEnabled = true, ilog::IMessageConsumer* logPtr = nullptr) override;

	// reimplemented (IJobOutput)
	virtual void SetProcessingReport(
				const ilog::IMessageContainer& processingLog,
				const QDateTime& startTime,
				const QDateTime& finishTime) override;
	virtual ProcessingOutputType GetOutputType(const QByteArray & outputObjectId) const override;
	virtual void SetOutputType(const QByteArray & outputObjectId, ProcessingOutputType outputType) override;
	virtual const IJobExecutionLog* GetExecutionLog() const override;
	virtual void SetExecutionLog(const IJobExecutionLog& executionLog) override;
	virtual const ilog::IMessageContainer* GetProcessorLog() const override;
	virtual void SetProcessorLog(const ilog::IMessageContainer& processorLog) override;

	// reimplemented (istd::IInformationProvider)
	virtual QDateTime GetInformationTimeStamp() const override;
	virtual InformationCategory GetInformationCategory() const override;
	virtual int GetInformationId() const override;
	virtual QString GetInformationDescription() const override;
	virtual QString GetInformationSource() const override;
	virtual int GetInformationFlags() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const istd::IChangeable& object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QString m_jobName;

	typedef QMap<QByteArray, ProcessingOutputType> ProcessingOutputTypeMap;
	ProcessingOutputTypeMap m_outputTypeMap;

	CProcessingInfo m_processingInfo;
	imtbase::CSimpleReferenceCollection m_results;
	
	// Separate logs for different purposes
	CJobExecutionLog m_executionLog;         // System-level controller events
	ilog::CMessageContainer m_processorLog;  // Worker processing output
};


} // namespace imthype


