// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CObjectCollection.h>


// ACF includes
#include <istd/TDelPtr.h>


namespace imtbase
{


// public methods

CObjectCollection::CObjectCollection()
	:m_operationFlags(IObjectCollection::OF_ALL & ~OF_SUPPORT_PAGINATION)
{
}


void CObjectCollection::SetOperationFlags(int flags, const QByteArray& objectId)
{
	if (objectId.isEmpty()){
		m_operationFlags = flags;
		return;
	}

	ObjectInfo* objectInfo = GetObjectInfo(objectId);
	if (objectInfo != nullptr){
		objectInfo->flags = flags;
	}
	else{
		m_operationFlags = flags;
	}
}


// reimplemented (imtbase::IObjectCollection)

int CObjectCollection::GetOperationFlags(const QByteArray& objectId) const
{
	ObjectInfo* objectInfo = GetObjectInfo(objectId);
	if (objectInfo != nullptr){
		return objectInfo->flags;
	}

	return m_operationFlags;
}


// reimplemented (istd::IChangeable)

int CObjectCollection::GetSupportedOperations() const
{
	return SO_CLONE | BaseClass::GetSupportedOperations();
}


istd::IChangeableUniquePtr CObjectCollection::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CObjectCollection());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


// protected methods

istd::IChangeableUniquePtr CObjectCollection::CreateObjectInstance(const QByteArray& typeId) const
{
	istd::IChangeableUniquePtr objPtr = BaseClass2::CreateInstance(typeId);

	return objPtr;
}


IObjectCollection* CObjectCollection::CreateSubCollectionInstance() const
{
	return new CObjectCollection;
}


} // namespace imtbase


