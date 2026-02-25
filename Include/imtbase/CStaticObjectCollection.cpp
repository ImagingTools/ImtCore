// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CStaticObjectCollection.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtbase/CObjectCollection.h>


namespace imtbase
{


// public methods

void CStaticObjectCollection::SetObject(const QByteArray& objectId, DataPtr objectPtr)
{
	ObjectInfo* objectInfoPtr = GetObjectInfo(objectId);
	if (objectInfoPtr != nullptr){
		objectInfoPtr->dataPtr.SetManagedPtr(objectPtr);
	}
}


QByteArray CStaticObjectCollection::RegisterObject(
			const QByteArray& objectId,
			const QByteArray& typeId,
			const QString& name,
			const QString& description,
			DataPtr objectPtr)
{
	ObjectInfo info;

	info.dataPtr.SetManagedPtr(objectPtr);
	info.description = description;
	info.name = name;
	info.typeId = typeId;
	info.flags = OF_SUPPORT_RENAME | OF_SUPPORT_EDIT;
	if (!objectId.isEmpty()){
		if (GetObjectInfo(objectId) == nullptr){
			info.id = objectId;
		}
	}

	istd::CChangeNotifier changeNotifier(this);

	if (InsertObjectIntoCollection(info)){
		changeNotifier.Abort();

		return info.id;
	}

	return QByteArray();
}


// reimplemented (IObjectCollection)

int CStaticObjectCollection::GetOperationFlags(const QByteArray& /*objectId*/) const
{
	return OF_SUPPORT_EDIT | OF_SUPPORT_RENAME;
}


QByteArray CStaticObjectCollection::InsertNewObject(
			const QByteArray& /*typeId*/,
			const QString& /*name*/,
			const QString& /*description*/,
			const istd::IChangeable* /*defaultValuePtr*/ ,
			const QByteArray& /*proposedObjectId*/,
			const idoc::IDocumentMetaInfo* /*dataMetaInfoPtr*/,
			const idoc::IDocumentMetaInfo* /*collectionItemMetaInfoPtr*/,
			const IOperationContext* /*operationContextPtr*/)
{
	return QByteArray();
}


// reimplemented (istd::IChangeable)

int CStaticObjectCollection::GetSupportedOperations() const
{
	return SO_CLONE | BaseClass::GetSupportedOperations();
}


istd::IChangeableUniquePtr CStaticObjectCollection::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CStaticObjectCollection());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


// protected methods

istd::IChangeableUniquePtr CStaticObjectCollection::CreateObjectInstance(const QByteArray& /*typeId*/) const
{
	return nullptr;
}


IObjectCollection* CStaticObjectCollection::CreateSubCollectionInstance() const
{
	return new CObjectCollection;
}


} // namespace imtbase


