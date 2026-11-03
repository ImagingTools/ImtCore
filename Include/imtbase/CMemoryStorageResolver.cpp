// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CMemoryStorageResolver.h>


// Qt includes
#include <QtCore/QUuid>


namespace imtbase
{


int CMemoryStorageResolver::GetObjectsCount() const
{
	QReadLocker locker(&m_lock);

	return m_objects.size();
}


// reimplemented (imtbase::IStorageResolver)

istd::IChangeableSharedPtr CMemoryStorageResolver::LoadObject(const Id& objectId) const
{
	QReadLocker locker(&m_lock);

	return m_objects.value(objectId);
}


bool CMemoryStorageResolver::SaveObject(const Id& objectId, const istd::IChangeable& object)
{
	istd::IChangeableSharedPtr storedPtr = LoadObject(objectId);
	if (!storedPtr.IsValid()){
		return false;
	}

	// The stored instance is live, so there is nothing to do if the object is the stored one.
	if (storedPtr.GetPtr() == &object){
		return true;
	}

	return storedPtr->CopyFrom(object);
}


IStorageResolver::Id CMemoryStorageResolver::InsertObject(const istd::IChangeableSharedPtr& objectPtr, const QString& /*name*/)
{
	if (!objectPtr.IsValid()){
		return Id();
	}

	const Id objectId = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);

	QWriteLocker locker(&m_lock);

	m_objects.insert(objectId, objectPtr);

	return objectId;
}


bool CMemoryStorageResolver::RemoveObject(const Id& objectId)
{
	QWriteLocker locker(&m_lock);

	return static_cast<bool>(m_objects.remove(objectId));
}


QVariantMap CMemoryStorageResolver::GetStorageParams(const Id& objectId) const
{
	QVariantMap params;
	params["storage"] = "memory";
	params["objectId"] = objectId;

	return params;
}


bool CMemoryStorageResolver::IsSnapshotStorage() const
{
	return false;
}


} // namespace imtbase


