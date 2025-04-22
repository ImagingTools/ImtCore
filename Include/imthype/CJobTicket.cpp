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


const iprm::IParamsSet* CJobTicket::GetParams() const
{
	return m_paramsPtr.GetPtr();
}


void CJobTicket::SetParams(iprm::IParamsSet* paramsPtr)
{
	m_paramsPtr.SetPtr(paramsPtr);
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
	if (m_progress != progress){
		istd::CChangeNotifier changeNotifier(this);

		m_progress = progress;
	}
}


const imthype::IJobOutput* CJobTicket::GetResults() const
{
	return &m_results;
}


void CJobTicket::SetResults(const imthype::IJobOutput& /*results*/)
{
}


const imtbase::IReferenceCollection* CJobTicket::GetInput() const
{
	return &m_input;
}


void CJobTicket::SetInput(const imtbase::IReferenceCollection& /*input*/)
{
}


// reimplemented (iser::ISerializable)

bool CJobTicket::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag uuidTag("Uuid", "Uuid", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(uuidTag);
	retVal = retVal && archive.Process(m_uuid);
	retVal = retVal && archive.EndTag(uuidTag);

	iser::CArchiveTag typeIdTag("TypeId", "Type-ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(typeIdTag);
	retVal = retVal && archive.Process(m_typeId);
	retVal = retVal && archive.EndTag(typeIdTag);

	iser::CArchiveTag nameTag("Name", "Name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(nameTag);
	retVal = retVal && archive.Process(m_name);
	retVal = retVal && archive.EndTag(nameTag);

	iser::CArchiveTag contextTag("Context", "Context", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(contextTag);
	retVal = retVal && archive.Process(m_contextId);
	retVal = retVal && archive.EndTag(contextTag);

	iser::CArchiveTag progressTag("Progress", "Progress", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(progressTag);
	retVal = retVal && archive.Process(m_progress);
	retVal = retVal && archive.EndTag(progressTag);

	iser::CArchiveTag processTag("Progress", "Progress", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(processTag);
	retVal = retVal && archive.Process(m_progress);
	retVal = retVal && archive.EndTag(processTag);

	return retVal;
}


} // namespace imthype


