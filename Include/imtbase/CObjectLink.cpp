#include <imtbase/CObjectLink.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtbase
{


void CObjectLink::SetObjectUuid(const QByteArray& objectUuid)
{
	if (m_objectUuid != objectUuid){
		istd::CChangeNotifier notifier(this);
		Q_UNUSED(notifier);

		m_objectUuid = objectUuid;
	}
}


void CObjectLink::SetFactoryId(const QByteArray& objectType)
{
	if (m_objectType != objectType){
		istd::CChangeNotifier notifier(this);
		Q_UNUSED(notifier);

		m_objectType = objectType;
	}
}


// reimplemented (IIdentifiable)

QByteArray CObjectLink::GetObjectUuid() const
{
	return m_objectUuid;
}


// reimplemented (iser::IObject)

QByteArray CObjectLink::GetFactoryId() const
{
	return m_objectType;
}


// reimplemented (iser::ISerializable)

bool CObjectLink::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);
	Q_UNUSED(notifier);

	bool retVal = true;

	static iser::CArchiveTag objectUuidTag("ObjectUuid", "Object-UUID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(objectUuidTag);
	retVal = retVal && archive.Process(m_objectUuid);
	retVal = retVal && archive.EndTag(objectUuidTag);

	static iser::CArchiveTag objectTypeTag("ObjectType", "Object Type", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(objectTypeTag);
	retVal = retVal && archive.Process(m_objectType);
	retVal = retVal && archive.EndTag(objectTypeTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CObjectLink::GetSupportedOperations() const
{
	return SO_CLONE | SO_COMPARE | SO_COPY | SO_OBSERVE | SO_RESET;
}


bool CObjectLink::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CObjectLink* sourcePtr = dynamic_cast<const CObjectLink*>(&object);
	if (sourcePtr != NULL){
		m_objectUuid = sourcePtr->m_objectUuid;
		m_objectType = sourcePtr->m_objectType;

		return true;
	}

	return false;
}


bool CObjectLink::IsEqual(const IChangeable& object) const
{
	const CObjectLink* sourcePtr = dynamic_cast<const CObjectLink*>(&object);
	if (sourcePtr != NULL){
		return m_objectUuid == sourcePtr->m_objectUuid &&
				m_objectType == sourcePtr->m_objectType;
	}

	return false;
}


istd::IChangeable* CObjectLink::CloneMe(istd::IChangeable::CompatibilityMode mode) const
{
	istd::TDelPtr<CObjectLink> clonePtr(new CObjectLink);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CObjectLink::ResetData(CompatibilityMode /*mode*/)
{
	m_objectUuid.clear();
	m_objectType.clear();

	return true;
}


} // namespace imtbase


