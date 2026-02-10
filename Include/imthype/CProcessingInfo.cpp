// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imthype/CProcessingInfo.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imthype
{


// public methods

CProcessingInfo::CProcessingInfo()
	:m_resultCategory(IC_NONE)
{
}


void CProcessingInfo::SetStartTime(const QDateTime& jobStartTime)
{
	if (m_startedAt != jobStartTime){
		istd::CChangeNotifier changeNotifier(this);

		m_startedAt = jobStartTime;
	}
}


void CProcessingInfo::SetFinishedTime(const QDateTime& finishedTime)
{
	if (m_resultTimeStamp != finishedTime){
		istd::CChangeNotifier changeNotifier(this);

		m_resultTimeStamp = finishedTime;
	}
}


void CProcessingInfo::SetResultCategory(istd::IInformationProvider::InformationCategory resultCategory)
{
	if (m_resultCategory != resultCategory){
		istd::CChangeNotifier changeNotifier(this);

		m_resultCategory = resultCategory;
	}
}


void CProcessingInfo::SetInformationDescription(const QString& informationDescription)
{
	if (m_informationDescription != informationDescription){
		istd::CChangeNotifier changeNotifier(this);

		m_informationDescription = informationDescription;
	}
}


void CProcessingInfo::SetProcessingLog(const ilog::IMessageContainer& processingLog)
{
	istd::CChangeNotifier changeNotifier(this);

	m_processingLog.CopyFrom(processingLog);
}


// reimplemented (IProcessingInfo)

CProcessingInfo::ProcessingInfo CProcessingInfo::GetProcessingInfo() const
{
	ProcessingInfo retVal;

	retVal.processingTime = m_startedAt.msecsTo(m_resultTimeStamp) / 1000.0;
	retVal.startedAt = m_startedAt;

	return retVal;
}


const ilog::IMessageContainer& CProcessingInfo::GetProcessingLog() const
{
	return m_processingLog;
}


// reimplemented (istd::IInformationProvider)

QDateTime CProcessingInfo::GetInformationTimeStamp() const
{
	return m_resultTimeStamp;
}


CProcessingInfo::InformationCategory CProcessingInfo::GetInformationCategory() const
{
	return m_resultCategory;
}


int CProcessingInfo::GetInformationId() const
{
	return -1;
}


QString CProcessingInfo::GetInformationDescription() const
{
	return m_informationDescription;
}


QString CProcessingInfo::GetInformationSource() const
{
	return QString();
}


int CProcessingInfo::GetInformationFlags() const
{
	return 0;
}


// reimplemented (iser::ISerializable)

bool CProcessingInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier changePtr(archive.IsStoring() ? NULL : this);

	bool retVal = true;

	if (!archive.IsStoring()){
		m_resultCategory = IC_NONE;
	}

	static iser::CArchiveTag startTimeTag("StartTime", "Processing start time", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(startTimeTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_startedAt);
	retVal = retVal && archive.EndTag(startTimeTag);

	static iser::CArchiveTag finishTimeTag("FinishTime", "Processing finish time", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(finishTimeTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_resultTimeStamp);
	retVal = retVal && archive.EndTag(finishTimeTag);

	static iser::CArchiveTag resultCategoryTag("ResultCategory", "Result of the processing", iser::CArchiveTag::TT_LEAF);
	int resultCategory = int(m_resultCategory);
	retVal = retVal && archive.BeginTag(resultCategoryTag);
	retVal = retVal && archive.Process(resultCategory);
	retVal = retVal && archive.EndTag(resultCategoryTag);

	static iser::CArchiveTag informationDescriptionTag("InformationDescription", "Information description", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(informationDescriptionTag);
	retVal = retVal && archive.Process(m_informationDescription);
	retVal = retVal && archive.EndTag(informationDescriptionTag);

	if (retVal && !archive.IsStoring()){
		m_resultCategory = InformationCategory(resultCategory);
	}

	static iser::CArchiveTag processingLogTag("ProcessingLog", "Processing log");
	retVal = retVal && archive.BeginTag(processingLogTag);
	retVal = m_processingLog.Serialize(archive);
	retVal = retVal && archive.EndTag(processingLogTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CProcessingInfo::CopyFrom(const istd::IChangeable& object, CompatibilityMode /*mode*/)
{
	const CProcessingInfo* sourcePtr = dynamic_cast<const CProcessingInfo*>(&object);
	if (sourcePtr != NULL){
		istd::CChangeNotifier changeNotifier(this);

		m_resultCategory = sourcePtr->m_resultCategory;
		m_startedAt = sourcePtr->m_startedAt;
		m_resultTimeStamp = sourcePtr->m_resultTimeStamp;
		m_informationDescription = sourcePtr->m_informationDescription;

		return m_processingLog.CopyFrom(*sourcePtr);
	}

	return false;
}


bool CProcessingInfo::IsEqual(const istd::IChangeable & object) const
{
	const CProcessingInfo* sourcePtr = dynamic_cast<const CProcessingInfo*>(&object);
	if (sourcePtr != NULL){
		bool retVal = m_processingLog.IsEqual(sourcePtr->m_processingLog);

		retVal = retVal && (m_resultCategory == sourcePtr->m_resultCategory);
		retVal = retVal && (m_startedAt == sourcePtr->m_startedAt);
		retVal = retVal && (m_resultTimeStamp == sourcePtr->m_resultTimeStamp);
		retVal = retVal && (m_informationDescription == sourcePtr->m_informationDescription);

		return retVal;
	}

	return false;
}


istd::IChangeableUniquePtr CProcessingInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr( new CProcessingInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return NULL;
}


bool CProcessingInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_processingLog.ClearMessages();

	m_resultCategory = istd::IInformationProvider::IC_NONE;
	m_startedAt = QDateTime();
	m_resultTimeStamp = QDateTime();
	m_informationDescription.clear();

	return true;
}


} // namespace imthype


