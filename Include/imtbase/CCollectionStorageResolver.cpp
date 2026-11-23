// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CCollectionStorageResolver.h>


namespace imtbase
{


CCollectionStorageResolver::CCollectionStorageResolver()
	:m_collectionPtr(nullptr)
{
}


CCollectionStorageResolver::CCollectionStorageResolver(IObjectCollection& collection)
	:m_collectionPtr(&collection)
{
}


void CCollectionStorageResolver::SetCollection(IObjectCollection* collectionPtr)
{
	m_collectionPtr = collectionPtr;
}


// reimplemented (imtbase::IStorageResolver)

istd::IChangeableSharedPtr CCollectionStorageResolver::LoadObject(const Id& objectId) const
{
	if (m_collectionPtr == nullptr){
		return istd::IChangeableSharedPtr();
	}

	IObjectCollection::DataPtr dataPtr;
	if (m_collectionPtr->GetObjectData(objectId, dataPtr) && dataPtr.IsValid()){
		return dataPtr;
	}

	return istd::IChangeableSharedPtr();
}


bool CCollectionStorageResolver::SaveObject(const Id& objectId, const istd::IChangeable& object)
{
	// A database collection reports success for an update of no rows, so an unknown ID is rejected here.
	if ((m_collectionPtr == nullptr) || m_collectionPtr->GetObjectTypeId(objectId).isEmpty()){
		return false;
	}

	return m_collectionPtr->SetObjectData(objectId, object);
}


IStorageResolver::Id CCollectionStorageResolver::InsertObject(
			const QByteArray& typeId,
			const istd::IChangeableSharedPtr& objectPtr,
			const QString& name)
{
	if ((m_collectionPtr == nullptr) || !objectPtr.IsValid()){
		return Id();
	}

	return m_collectionPtr->InsertNewObject(
		typeId,
		name,
		QString(),
		objectPtr.GetPtr()
	);
}


bool CCollectionStorageResolver::RemoveObject(const Id& objectId)
{
	if (m_collectionPtr == nullptr){
		return false;
	}

	return m_collectionPtr->RemoveElements(IObjectCollection::Ids() << objectId);
}


} // namespace imtbase


