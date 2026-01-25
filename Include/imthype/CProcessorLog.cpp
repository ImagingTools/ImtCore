#include <imthype/CProcessorLog.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imthype
{


CProcessorLog::CProcessorLog()
{
}


CProcessorLog::CProcessorLog(const QByteArray& processorId)
	:m_processorId(processorId)
{
}


QByteArray CProcessorLog::GetProcessorId() const
{
	return m_processorId;
}


void CProcessorLog::SetProcessorId(const QByteArray& processorId)
{
	if (m_processorId != processorId) {
		istd::CChangeNotifier changeNotifier(this);
		m_processorId = processorId;
	}
}


bool CProcessorLog::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);
	
	bool retVal = true;
	
	// Serialize the processor ID
	static iser::CArchiveTag processorIdTag("ProcessorId", "Processor identifier", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(processorIdTag);
	retVal = retVal && archive.Process(m_processorId);
	retVal = retVal && archive.EndTag(processorIdTag);
	
	// Serialize the message container
	static iser::CArchiveTag messagesTag("Messages", "Processor log messages");
	retVal = retVal && archive.BeginTag(messagesTag);
	retVal = retVal && ilog::CMessageContainer::Serialize(archive);
	retVal = retVal && archive.EndTag(messagesTag);
	
	return retVal;
}


bool CProcessorLog::CopyFrom(const istd::IChangeable& object, CompatibilityMode /*mode*/)
{
	const CProcessorLog* sourcePtr = dynamic_cast<const CProcessorLog*>(&object);
	if (sourcePtr) {
		istd::CChangeNotifier changeNotifier(this);
		
		m_processorId = sourcePtr->m_processorId;
		ilog::CMessageContainer::CopyFrom(*sourcePtr);
		
		return true;
	}
	
	return false;
}


bool CProcessorLog::IsEqual(const istd::IChangeable& object) const
{
	const CProcessorLog* sourcePtr = dynamic_cast<const CProcessorLog*>(&object);
	if (sourcePtr) {
		bool retVal = (m_processorId == sourcePtr->m_processorId);
		retVal = retVal && ilog::CMessageContainer::IsEqual(*sourcePtr);
		
		return retVal;
	}
	
	return false;
}


istd::IChangeableUniquePtr CProcessorLog::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CProcessorLog);
	if (clonePtr->CopyFrom(*this, mode)) {
		return clonePtr;
	}
	
	return nullptr;
}


bool CProcessorLog::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);
	
	m_processorId.clear();
	ilog::CMessageContainer::Clear();
	
	return true;
}


} // namespace imthype


