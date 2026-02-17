// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial

#include <imthype/CStandardJobOutput.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imthype
{


// public methods

void CStandardJobOutput::SetJobName(const QString& jobName)
{
	if (m_jobName != jobName){
		istd::CChangeNotifier changeNotifier(this);

		m_jobName = jobName;
	}
}


void CStandardJobOutput::SetStartTime(const QDateTime& startTime)
{
	return m_processingInfo.SetStartTime(startTime);
}


void CStandardJobOutput::SetFinishedTime(const QDateTime& finishedTime)
{
	return m_processingInfo.SetFinishedTime(finishedTime);
}


void CStandardJobOutput::SetResultCategory(istd::IInformationProvider::InformationCategory resultCategory)
{
	return m_processingInfo.SetResultCategory(resultCategory);
}


// reimplemented (IProcessingInfo)

IJobOutput::ProcessingInfo CStandardJobOutput::GetProcessingInfo() const
{
	return m_processingInfo.GetProcessingInfo();
}


const ilog::IMessageContainer& CStandardJobOutput::GetProcessingLog() const
{
	return m_processingInfo.GetProcessingLog();
}


// reimplemented (imtbase::IReferenceCollection)

imtbase::IReferenceCollection::ICollectionObjectLinkPtr CStandardJobOutput::GetObjectLink(const Id& objectId, const QByteArray& repositoryId) const
{
	return m_results.GetObjectLink(objectId, repositoryId);
}


bool CStandardJobOutput::InsertReference(const Id& objectId, const QByteArray& repositoryId)
{
	istd::CChangeNotifier changeNotfier(this);

	return m_results.InsertReference(objectId, repositoryId);
}


bool CStandardJobOutput::RemoveReference(const Id& objectId)
{
	istd::CChangeNotifier changeNotfier(this);

	return m_results.RemoveReference(objectId);
}


bool CStandardJobOutput::RemoveAllReferences()
{
	istd::CChangeNotifier changeNotfier(this);

	return m_results.RemoveAllReferences();
}


// reimplemented (imtbase::IObjectCollectionInfo)

const iprm::IOptionsList* CStandardJobOutput::GetObjectTypesInfo() const
{
	return m_results.GetObjectTypesInfo();
}


QByteArray CStandardJobOutput::GetObjectTypeId(const Id& objectId) const
{
	return m_results.GetObjectTypeId(objectId);
}


idoc::MetaInfoPtr CStandardJobOutput::GetDataMetaInfo(const Id& objectId) const
{
	return m_results.GetDataMetaInfo(objectId);
}


// reimplemented (imtbase::ICollectionInfo)

int CStandardJobOutput::GetElementsCount(const iprm::IParamsSet* selectionParamPtr, ilog::IMessageConsumer* logPtr) const
{
	return m_results.GetElementsCount(selectionParamPtr, logPtr);
}


imtbase::ICollectionInfo::Ids CStandardJobOutput::GetElementIds(
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr,
			ilog::IMessageConsumer* logPtr) const
{
	return m_results.GetElementIds(offset, count, selectionParamsPtr, logPtr);
}


bool CStandardJobOutput::GetSubsetInfo(
			ICollectionInfo& subsetInfo,
			int offset,
			int count,
			const iprm::IParamsSet* selectionParamsPtr,
			ilog::IMessageConsumer* logPtr) const
{
	return m_results.GetSubsetInfo(subsetInfo, offset, count, selectionParamsPtr, logPtr);
}


QVariant CStandardJobOutput::GetElementInfo(const Id& elementId, int infoType, ilog::IMessageConsumer* logPtr) const
{
	return m_results.GetElementInfo(elementId, infoType, logPtr);
}


idoc::MetaInfoPtr CStandardJobOutput::GetElementMetaInfo(const Id& elementId, ilog::IMessageConsumer* logPtr) const
{
	return m_results.GetElementMetaInfo(elementId, logPtr);
}


bool CStandardJobOutput::SetElementName(const Id& elementId, const QString& name, ilog::IMessageConsumer* logPtr)
{
	return m_results.SetElementName(elementId, name, logPtr);
}


bool CStandardJobOutput::SetElementDescription(const Id& elementId, const QString& description, ilog::IMessageConsumer* logPtr)
{
	return m_results.SetElementDescription(elementId, description, logPtr);
}


bool CStandardJobOutput::SetElementEnabled(const Id& elementId, bool isEnabled, ilog::IMessageConsumer* logPtr)
{
	return m_results.SetElementEnabled(elementId, isEnabled, logPtr);
}


// reimplemented (IJobOutput)

void CStandardJobOutput::SetProcessingReport(
			const ilog::IMessageContainer& processingLog,
			const QDateTime& startTime,
			const QDateTime& finishTime)
{
	istd::CChangeGroup changeGroup(this);

	m_processingInfo.SetStartTime(startTime);
	m_processingInfo.SetFinishedTime(finishTime);
	m_processingInfo.SetProcessingLog(processingLog);
}


IJobOutput::ProcessingOutputType CStandardJobOutput::GetOutputType(const QByteArray& outputObjectId) const
{
	if (m_outputTypeMap.contains(outputObjectId)){
		return m_outputTypeMap[outputObjectId];
	}

	return POT_RESULT;
}


void CStandardJobOutput::SetOutputType(const QByteArray& outputObjectId, ProcessingOutputType outputType)
{
	istd::CChangeNotifier changeNotifier(this);

	m_outputTypeMap[outputObjectId] = outputType;
}


const IJobExecutionLog* CStandardJobOutput::GetExecutionLog() const
{
	return &m_executionLog;
}


void CStandardJobOutput::SetExecutionLog(const IJobExecutionLog& executionLog)
{
	istd::CChangeNotifier changeNotifier(this);
	
	// Copy the execution log using CopyFrom
	const istd::IChangeable* changeablePtr = dynamic_cast<const istd::IChangeable*>(&executionLog);
	if (changeablePtr) {
		m_executionLog.CopyFrom(*changeablePtr);
	}
}


const ilog::IMessageContainer* CStandardJobOutput::GetProcessorLog() const
{
	return &m_processorLog;
}


void CStandardJobOutput::SetProcessorLog(const ilog::IMessageContainer& processorLog)
{
	istd::CChangeNotifier changeNotifier(this);
	
	// Copy the processor log using CopyFrom (IMessageContainer inherits from IChangeable)
	m_processorLog.CopyFrom(processorLog);
}


// reimplemented (istd::IInformationProvider)

QDateTime CStandardJobOutput::GetInformationTimeStamp() const
{
	return m_processingInfo.GetInformationTimeStamp();
}


istd::IInformationProvider::InformationCategory CStandardJobOutput::GetInformationCategory() const
{
	return m_processingInfo.GetInformationCategory();
}


int CStandardJobOutput::GetInformationId() const
{
	return m_processingInfo.GetInformationId();
}


QString CStandardJobOutput::GetInformationDescription() const
{
	return m_processingInfo.GetInformationDescription();
}


QString CStandardJobOutput::GetInformationSource() const
{
	return m_jobName;
}

int CStandardJobOutput::GetInformationFlags() const
{
	return m_processingInfo.GetInformationFlags();
}


// reimplemented (iser::ISerializable)

bool CStandardJobOutput::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier changePtr(archive.IsStoring() ? NULL : this);

	bool retVal = true;

	static iser::CArchiveTag processingInfoTag("ProcessingInfo", "Processing informations", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(processingInfoTag);
	retVal = retVal && m_processingInfo.Serialize(archive);
	retVal = retVal && archive.EndTag(processingInfoTag);

	static iser::CArchiveTag jobNameTag("JobName", "Job name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(jobNameTag);
	retVal = retVal && archive.Process(m_jobName);
	retVal = retVal && archive.EndTag(jobNameTag);

	static iser::CArchiveTag outputTag("Output", "Processing output", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(outputTag);
	retVal = retVal && m_results.Serialize(archive);
	retVal = retVal && archive.EndTag(outputTag);

	static iser::CArchiveTag executionLogTag("ExecutionLog", "Job execution log", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(executionLogTag);
	retVal = retVal && m_executionLog.Serialize(archive);
	retVal = retVal && archive.EndTag(executionLogTag);

	static iser::CArchiveTag processorLogTag("ProcessorLog", "Processor/worker log", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(processorLogTag);
	retVal = retVal && m_processorLog.Serialize(archive);
	retVal = retVal && archive.EndTag(processorLogTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CStandardJobOutput::CopyFrom(const istd::IChangeable& object, CompatibilityMode /*mode*/)
{
	const CStandardJobOutput* sourcePtr = dynamic_cast<const CStandardJobOutput*>(&object);
	if (sourcePtr != NULL){
		istd::CChangeNotifier changeNotifier(this);

		m_processingInfo.CopyFrom(sourcePtr->m_processingInfo);
		m_results.CopyFrom(sourcePtr->m_results);

		m_jobName = sourcePtr->m_jobName;
		m_outputTypeMap = sourcePtr->m_outputTypeMap;
		
		// Copy logs (CopyFrom can fail, check return value)
		if (!m_executionLog.CopyFrom(sourcePtr->m_executionLog)) {
			return false;
		}
		if (!m_processorLog.CopyFrom(sourcePtr->m_processorLog)) {
			return false;
		}

		return true;
	}

	return false;
}


bool CStandardJobOutput::IsEqual(const istd::IChangeable& object) const
{
	const CStandardJobOutput* sourcePtr = dynamic_cast<const CStandardJobOutput*>(&object);
	if (sourcePtr != NULL){
		bool retVal = m_processingInfo.IsEqual(sourcePtr->m_processingInfo);
		retVal = retVal && m_results.IsEqual(sourcePtr->m_results);

		retVal = retVal && (m_jobName == sourcePtr->m_jobName);
		retVal = retVal && (m_outputTypeMap == sourcePtr->m_outputTypeMap);
		
		// Compare logs
		retVal = retVal && m_executionLog.IsEqual(sourcePtr->m_executionLog);
		retVal = retVal && m_processorLog.IsEqual(sourcePtr->m_processorLog);

		return retVal;
	}

	return false;
}


istd::IChangeableUniquePtr CStandardJobOutput::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CStandardJobOutput);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return NULL;
}


bool CStandardJobOutput::ResetData(CompatibilityMode mode)
{
	istd::CChangeNotifier changeNotifier(this);

	m_processingInfo.ResetData();
	m_results.ResetData();

	m_jobName.clear();
	m_outputTypeMap.clear();
	
	// Reset logs using ResetData
	m_executionLog.ResetData(mode);
	m_processorLog.ResetData(mode);

	return true;
}


} // namespace imthype


