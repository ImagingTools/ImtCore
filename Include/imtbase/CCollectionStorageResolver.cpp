// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CCollectionStorageResolver.h>


namespace imtbase
{


CCollectionStorageResolver::CCollectionStorageResolver(IObjectCollection& collection, const QByteArray& typeId)
	:m_collection(collection),
	m_typeId(typeId)
{
}


// reimplemented (imtbase::IStorageResolver)

istd::IChangeableSharedPtr CCollectionStorageResolver::LoadObject(const Id& objectId) const
{
	IObjectCollection::DataPtr dataPtr;
	if (m_collection.GetObjectData(objectId, dataPtr) && dataPtr.IsValid()){
		return dataPtr;
	}

	return istd::IChangeableSharedPtr();
}


bool CCollectionStorageResolver::SaveObject(const Id& objectId, const istd::IChangeable& object)
{
	return m_collection.SetObjectData(objectId, object);
}


IStorageResolver::Id CCollectionStorageResolver::InsertObject(const istd::IChangeableSharedPtr& objectPtr, const QString& name)
{
	if (!objectPtr.IsValid()){
		return Id();
	}

	return m_collection.InsertNewObject(m_typeId, name, QString(), objectPtr.GetPtr());
}


bool CCollectionStorageResolver::RemoveObject(const Id& objectId)
{
	return m_collection.RemoveElements(IObjectCollection::Ids() << objectId);
}


QVariantMap CCollectionStorageResolver::GetStorageParams(const Id& objectId) const
{
	QVariantMap params;
	params["storage"] = "collection";
	params["objectId"] = objectId;
	params["typeId"] = m_collection.GetObjectTypeId(objectId);
	params["name"] = m_collection.GetElementInfo(objectId, IObjectCollection::EIT_NAME);

	return params;
}


bool CCollectionStorageResolver::IsSnapshotStorage() const
{
	return true;
}


} // namespace imtbase


