// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CObjectLink.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtbase
{


void CObjectLink::SetFactoryId(const QByteArray& objectType)
{
	if (m_objectTypeId != objectType){
		istd::CChangeNotifier notifier(this);
		Q_UNUSED(notifier);

		m_objectTypeId = objectType;
	}
}


void CObjectLink::SetCollectionPtr(const imtbase::IObjectCollection* collectionPtr)
{
	m_collectionPtr = collectionPtr;
}


// reimplemented (ICollectionObjectLink)

const imtbase::IObjectCollection* CObjectLink::GetCollection() const
{
	return m_collectionPtr;
}


// reimplemented (IIdentifiable)

QByteArray CObjectLink::GetObjectUuid() const
{
	return m_objectUuid;
}


bool CObjectLink::SetObjectUuid(const QByteArray& objectUuid)
{
	if (m_objectUuid != objectUuid){
		istd::CChangeNotifier notifier(this);
		Q_UNUSED(notifier);

		m_objectUuid = objectUuid;
	}

	return true;
}


// reimplemented (iser::IObject)

QByteArray CObjectLink::GetFactoryId() const
{
	return m_objectTypeId;
}


// reimplemented (iser::ISerializable)

bool CObjectLink::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);
	Q_UNUSED(notifier);

	bool retVal = true;

	iser::CArchiveTag objectUuidTag("ObjectUuid", "Object-UUID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(objectUuidTag);
	retVal = retVal && archive.Process(m_objectUuid);
	retVal = retVal && archive.EndTag(objectUuidTag);

	iser::CArchiveTag objectTypeTag("ObjectType", "Object Type", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(objectTypeTag);
	retVal = retVal && archive.Process(m_objectTypeId);
	retVal = retVal && archive.EndTag(objectTypeTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CObjectLink::GetSupportedOperations() const
{
	return SO_CLONE | SO_COMPARE | SO_COPY | SO_RESET;
}


bool CObjectLink::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CObjectLink* sourcePtr = dynamic_cast<const CObjectLink*>(&object);
	if (sourcePtr != NULL){
		istd::CChangeNotifier changeNotifier(this);

		m_objectUuid = sourcePtr->m_objectUuid;
		m_objectTypeId = sourcePtr->m_objectTypeId;

		return true;
	}

	return false;
}


bool CObjectLink::IsEqual(const IChangeable& object) const
{
	const CObjectLink* sourcePtr = dynamic_cast<const CObjectLink*>(&object);
	if (sourcePtr != NULL){
		return m_objectUuid == sourcePtr->m_objectUuid &&
				m_objectTypeId == sourcePtr->m_objectTypeId;
	}

	return false;
}


istd::IChangeableUniquePtr CObjectLink::CloneMe(istd::IChangeable::CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CObjectLink);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CObjectLink::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_objectUuid.clear();
	m_objectTypeId.clear();

	return true;
}


} // namespace imtbase


