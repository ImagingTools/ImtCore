// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CCollectionStorageResolver.h>


namespace imtbase
{


CCollectionStorageResolver::CCollectionStorageResolver(
			IObjectCollection& collection,
			const QByteArray& typeId,
			const iser::IVersionInfo* versionInfoPtr)
	:m_collectionPtr(&collection),
	m_typeId(typeId),
	m_versionInfoPtr(versionInfoPtr)
{
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
	if (m_collectionPtr == nullptr){
		return false;
	}

	/**
		Collections differ for a missing object: the in-memory, file and remote ones fail, a database table delegate
		reports success for an UPDATE of no rows, and the JSON document delegate may insert a new row.
		The type-ID is the existence check available for all of them. In a database it still reads the row,
		but this happens only for a changed object: StorageLink doesn't save an unchanged one.
	*/
	if (m_collectionPtr->GetObjectTypeId(objectId).isEmpty()){
		return false;
	}

	return m_collectionPtr->SetObjectData(objectId, object);
}


IStorageResolver::Id CCollectionStorageResolver::InsertObject(const istd::IChangeableSharedPtr& objectPtr, const QString& name)
{
	if ((m_collectionPtr == nullptr) || !objectPtr.IsValid()){
		return Id();
	}

	return m_collectionPtr->InsertNewObject(m_typeId, name, QString(), objectPtr.GetPtr());
}


bool CCollectionStorageResolver::RemoveObject(const Id& objectId)
{
	if (m_collectionPtr == nullptr){
		return false;
	}

	return m_collectionPtr->RemoveElements(IObjectCollection::Ids() << objectId);
}


QVariantMap CCollectionStorageResolver::GetStorageParams(const Id& objectId) const
{
	QVariantMap params;
	params[QStringLiteral("storage")] = QStringLiteral("collection");
	params[QStringLiteral("objectId")] = objectId;

	if (m_collectionPtr != nullptr){
		params[QStringLiteral("typeId")] = m_collectionPtr->GetObjectTypeId(objectId);
		params[QStringLiteral("name")] = m_collectionPtr->GetElementInfo(objectId, IObjectCollection::EIT_NAME);
	}

	return params;
}


bool CCollectionStorageResolver::IsSnapshotStorage() const
{
	return true;
}


const iser::IVersionInfo* CCollectionStorageResolver::GetVersionInfo() const
{
	return m_versionInfoPtr;
}


// protected methods

CCollectionStorageResolver::CCollectionStorageResolver()
	:m_collectionPtr(nullptr),
	m_versionInfoPtr(nullptr)
{
}


void CCollectionStorageResolver::SetCollection(
			IObjectCollection* collectionPtr,
			const QByteArray& typeId,
			const iser::IVersionInfo* versionInfoPtr)
{
	m_collectionPtr = collectionPtr;
	m_typeId = typeId;
	m_versionInfoPtr = versionInfoPtr;
}


} // namespace imtbase


