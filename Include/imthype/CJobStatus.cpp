// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imthype/CJobStatus.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imthype
{


// public methods

CJobStatus::CJobStatus()
	:m_processingStatus(IJobQueueManager::PS_NONE),
	m_progress(0.0)
{
}


CJobStatus::CJobStatus(const QByteArray& jobId)
	:m_jobId(jobId),
	m_processingStatus(IJobQueueManager::PS_NONE),
	m_progress(0.0)
{
}


// reimplemented (imthype::IJobStatus)

IJobQueueManager::ProcessingStatus CJobStatus::GetProcessingStatus() const
{
	return m_processingStatus;
}


void CJobStatus::SetProcessingStatus(IJobQueueManager::ProcessingStatus status)
{
	if (m_processingStatus != status){
		istd::CChangeNotifier changeNotifier(this);
		m_processingStatus = status;
	}
}


double CJobStatus::GetProgress() const
{
	return m_progress;
}


void CJobStatus::SetProgress(double progress)
{
	if (m_progress != progress){
		istd::CChangeNotifier changeNotifier(this);
		m_progress = progress;
	}
}


// reimplemented (istd::IChangeable)

bool CJobStatus::CopyFrom(const IChangeable& object, CompatibilityMode mode)
{
	Q_UNUSED(mode);

	const CJobStatus* sourcePtr = dynamic_cast<const CJobStatus*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_jobId = sourcePtr->m_jobId;
		m_processingStatus = sourcePtr->m_processingStatus;
		m_progress = sourcePtr->m_progress;

		return true;
	}

	return false;
}


int CJobStatus::GetSupportedOperations() const
{
	return SO_COPY | SO_CLONE | SO_COMPARE | SO_RESET;
}


bool CJobStatus::IsEqual(const IChangeable& object) const
{
	const CJobStatus* implPtr = dynamic_cast<const CJobStatus*>(&object);
	if (implPtr != nullptr) {
		return
			(m_jobId == implPtr->m_jobId) &&
			(m_processingStatus == implPtr->m_processingStatus) &&
			(qFuzzyIsNull(m_progress - implPtr->m_progress));
	}

	return false;
}


istd::IChangeableUniquePtr CJobStatus::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CJobStatus);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CJobStatus::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_jobId.clear();
	m_processingStatus = IJobQueueManager::PS_NONE;
	m_progress = 0.0;

	return true;
}


// reimplemented (iser::ISerializable)

bool CJobStatus::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	static iser::CArchiveTag jobIdTag("JobId", "Job UUID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(jobIdTag);
	retVal = retVal && archive.Process(m_jobId);
	retVal = retVal && archive.EndTag(jobIdTag);

	static iser::CArchiveTag statusTag("Status", "Processing status", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(statusTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeEnum<
				IJobQueueManager::ProcessingStatus,
				IJobQueueManager::ToString,
				IJobQueueManager::FromString>(archive, m_processingStatus);
	retVal = retVal && archive.EndTag(statusTag);

	static iser::CArchiveTag progressTag("Progress", "Processing progress", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(progressTag);
	retVal = retVal && archive.Process(m_progress);
	retVal = retVal && archive.EndTag(progressTag);

	return retVal;
}


} // namespace imthype


