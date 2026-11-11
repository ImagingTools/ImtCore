// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CCollectionStorageResolver.h>


namespace imtbase
{


CCollectionStorageResolver::CCollectionStorageResolver(IObjectCollection& collection, const QByteArray& typeId)
	:m_collectionPtr(&collection),
	m_typeId(typeId)
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


// protected methods

CCollectionStorageResolver::CCollectionStorageResolver()
	:m_collectionPtr(nullptr)
{
}


void CCollectionStorageResolver::SetCollection(IObjectCollection* collectionPtr, const QByteArray& typeId)
{
	m_collectionPtr = collectionPtr;
	m_typeId = typeId;
}


} // namespace imtbase


