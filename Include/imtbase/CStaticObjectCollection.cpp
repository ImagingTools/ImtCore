#include <imtbase/CStaticObjectCollection.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>


namespace imtbase
{


// public methods

void CStaticObjectCollection::SetObject(const QByteArray& objectId, istd::IChangeable* objectPtr)
{
	ObjectInfo* objectInfoPtr = GetObjectInfo(objectId);
	if (objectInfoPtr != nullptr){
		objectInfoPtr->objectPtr.SetPtr(objectPtr, false);
	}
}


QByteArray CStaticObjectCollection::RegisterObject(
			const QByteArray& objectId,
			const QByteArray& typeId,
			const QString& name,
			const QString& description,
			istd::IChangeable* objectPtr)
{
	ObjectInfo info;

	info.objectPtr.SetPtr(objectPtr, false);
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

int CStaticObjectCollection::GetOperationFlags(const QByteArray & /*objectId*/) const
{
	return OF_SUPPORT_EDIT | OF_SUPPORT_RENAME;
}


QByteArray CStaticObjectCollection::InsertNewObject(
			const QByteArray& /*typeId*/,
			const QString& /*name*/,
			const QString& /*description*/,
			const istd::IChangeable* /*defaultValuePtr*/,
			const QByteArray& /*proposedObjectId*/)
{
	return QByteArray();
}


// reimplemented (istd::IChangeable)

int CStaticObjectCollection::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


istd::IChangeable* CStaticObjectCollection::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CStaticObjectCollection> clonePtr(new CStaticObjectCollection());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


// protected methods

istd::IChangeable* CStaticObjectCollection::CreateObjectInstance(const QByteArray& /*typeId*/) const
{
	return nullptr;
}


} // namespace imtbase


