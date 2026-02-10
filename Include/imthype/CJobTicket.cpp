// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imthype/CJobTicket.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imthype
{


// public methods

CJobTicket::CJobTicket()
	:m_progress(0.0),
	m_processingStatus(IJobQueueManager::PS_NONE)
{
}


// reimplemented (imthype::IJobTicket)

QByteArray CJobTicket::GetTypeId() const
{
	return m_typeId;
}


void CJobTicket::SetTypeId(const QByteArray& typeId)
{
	if (m_typeId != typeId){
		istd::CChangeNotifier changeNotifier(this);

		m_typeId = typeId;
	}
}


QByteArray CJobTicket::GetUuid() const
{
	return m_uuid;
}


void CJobTicket::SetUuid(const QByteArray& uuid)
{
	if (m_uuid != uuid){
		istd::CChangeNotifier changeNotifier(this);

		m_uuid = uuid;
	}
}


QString CJobTicket::GetJobName() const
{
	return m_name;
}


void CJobTicket::SetJobName(const QString& name)
{
	if (m_name != name){
		istd::CChangeNotifier changeNotifier(this);

		m_name = name;
	}
}


QByteArray CJobTicket::GetContextId() const
{
	return m_contextId;
}


void CJobTicket::SetContextId(const QByteArray& contextId)
{
	if (m_contextId != contextId){
		istd::CChangeNotifier changeNotifier(this);

		m_contextId = contextId;
	}
}


iprm::IParamsSetSharedPtr CJobTicket::GetParams() const
{
	return m_paramsPtr;
}


iprm::IParamsSetSharedPtr CJobTicket::CreateParams() const
{
	iprm::IParamsSetSharedPtr retVal;
	if (m_paramsFactory){
		retVal = m_paramsFactory(m_contextId, m_typeId);
	}

	return retVal;
}


void CJobTicket::SetParams(const iprm::IParamsSetSharedPtr& paramsPtr)
{
	m_paramsPtr = paramsPtr;
}


imthype::IJobQueueManager::ProcessingStatus CJobTicket::GetProcessingStatus() const
{
	return m_processingStatus;
}


void CJobTicket::SetProcessingStatus(imthype::IJobQueueManager::ProcessingStatus status)
{
	if (m_processingStatus != status){
		istd::CChangeNotifier changeNotifier(this);

		m_processingStatus = status;
	}
}


double CJobTicket::GetProgress() const
{
	return m_progress;
}


void CJobTicket::SetProgress(double progress)
{
	if (!qFuzzyCompare(m_progress, progress)){
		istd::CChangeNotifier changeNotifier(this);

		m_progress = progress;
	}
}


const imthype::IJobOutput* CJobTicket::GetResults() const
{
	return &m_results;
}


void CJobTicket::SetResults(const imthype::IJobOutput& results)
{
	istd::CChangeNotifier changeNotifier(this);

	m_results.CopyFrom(results);
}


const imtbase::IReferenceCollection* CJobTicket::GetInput() const
{
	return &m_input;
}


void CJobTicket::SetInput(const imtbase::IReferenceCollection& input)
{
	istd::CChangeNotifier changeNotifier(this);

	m_input.CopyFrom(input);
}


void CJobTicket::SetParamsFactory(const ParamsFactoryFunction& factory)
{
	m_paramsFactory = factory;
}


// reimplemented (iser::ISerializable)

bool CJobTicket::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	static iser::CArchiveTag contextIdTag("ContextId", "ID of the job context", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(contextIdTag);
	retVal = retVal && archive.Process(m_contextId);
	retVal = retVal && archive.EndTag(contextIdTag);

	static iser::CArchiveTag uuidTag("Uuid", "UUID of the job", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(uuidTag);
	retVal = retVal && archive.Process(m_uuid);
	retVal = retVal && archive.EndTag(uuidTag);

	static iser::CArchiveTag typeIdTag("TypeId", "Processing type-ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(typeIdTag);
	retVal = retVal && archive.Process(m_typeId);
	retVal = retVal && archive.EndTag(typeIdTag);

	static iser::CArchiveTag nameTag("Name", "Name of the job", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(nameTag);
	retVal = retVal && archive.Process(m_name);
	retVal = retVal && archive.EndTag(nameTag);

	static iser::CArchiveTag statusTag("Status", "Processing status", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(statusTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeEnum<IJobQueueManager::ProcessingStatus,
																	  IJobQueueManager::ToString,
																	  IJobQueueManager::FromString>(archive, m_processingStatus);
	retVal = retVal && archive.EndTag(statusTag);

	static iser::CArchiveTag progressTag("Progress", "Processing progress", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(progressTag);
	retVal = retVal && archive.Process(m_progress);
	retVal = retVal && archive.EndTag(progressTag);

	static iser::CArchiveTag inputTag("Input", "Job input", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(inputTag);
	retVal = retVal && m_input.Serialize(archive);
	retVal = retVal && archive.EndTag(inputTag);

	// Handle params serialization/deserialization
	static iser::CArchiveTag paramsTag("Configuration", "Processing parameters", iser::CArchiveTag::TT_GROUP);
	if (retVal && archive.BeginTag(paramsTag)){
		// Create params object during deserialization if factory is set
		if (m_paramsFactory && !archive.IsStoring() && !m_paramsPtr.IsValid()){
			m_paramsPtr = m_paramsFactory(m_contextId, m_typeId);
		}
		
		if (m_paramsPtr.IsValid()){
			retVal = retVal && m_paramsPtr->Serialize(archive);
		}
		
		retVal = retVal && archive.EndTag(paramsTag);
	}

	static iser::CArchiveTag resultsTag("Results", "Job results", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(resultsTag);
	retVal = retVal && m_results.Serialize(archive);
	retVal = retVal && archive.EndTag(resultsTag);

	return retVal;
}


bool CJobTicket::CopyFrom(const IChangeable &object, CompatibilityMode mode)
{
	Q_UNUSED(mode);

	const CJobTicket* sourcePtr = dynamic_cast<const CJobTicket*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_typeId = sourcePtr->m_typeId;
		m_uuid = sourcePtr->m_uuid;
		m_name = sourcePtr->m_name;
		m_contextId = sourcePtr->m_contextId;
		m_progress = sourcePtr->m_progress;
		m_processingStatus = sourcePtr->m_processingStatus;
		m_results = sourcePtr->m_results;
		m_input = sourcePtr->m_input;
		m_paramsPtr = sourcePtr->m_paramsPtr;
		m_paramsFactory = sourcePtr->m_paramsFactory;

		return true;
	}

	return false;
}


} // namespace imthype


