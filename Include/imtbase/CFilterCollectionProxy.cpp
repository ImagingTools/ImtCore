#include <imtbase/CFilterCollectionProxy.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>


namespace imtbase
{


// public methods

CFilterCollectionProxy::CFilterCollectionProxy(IObjectCollection& parent, bool isObjectProxy)
	:m_operationFlags(IObjectCollection::OF_ALL & ~OF_SUPPORT_PAGINATION),
	 m_parent(parent),
	 m_isObjectProxy(isObjectProxy)
{
}


void CFilterCollectionProxy::SetOperationFlags(int flags, const QByteArray& objectId)
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

int CFilterCollectionProxy::GetOperationFlags(const QByteArray& objectId) const
{
	ObjectInfo* objectInfo = GetObjectInfo(objectId);
	if (objectInfo != nullptr){
		return objectInfo->flags;
	}

	return m_operationFlags;
}


ICollectionInfo::Id CFilterCollectionProxy::InsertNewObject(
			const QByteArray& typeId,
			const QString& name,
			const QString& description,
			DataPtr defaultValuePtr,
			const Id& proposedElementId,
			const idoc::IDocumentMetaInfo* dataMetaInfoPtr,
			const idoc::IDocumentMetaInfo* elementMetaInfoPtr,
			const IOperationContext* /*operationContextPtr*/)
{
	if (m_isObjectProxy){
		return BaseClass::InsertNewObject(
					typeId,
					name,
					description,
					defaultValuePtr,
					proposedElementId,
					dataMetaInfoPtr,
					elementMetaInfoPtr);
	}

	ObjectInfo info;

	info.description = description;
	info.name = name;
	info.typeId = typeId;
	info.flags = GetItemDefaultFlags();
	if (!proposedElementId.isEmpty()){
		info.id = proposedElementId;
	}

	if (elementMetaInfoPtr != nullptr){
		info.collectionItemMetaInfo.CopyFrom(*elementMetaInfoPtr);
	}

	if (dataMetaInfoPtr != nullptr){
		info.contentsMetaInfoPtr.SetCastedOrRemove(dataMetaInfoPtr->CloneMe());
	}

	if (InsertObjectIntoCollection(info)){
		istd::IChangeable::ChangeSet changeSet(CF_ADDED);
		changeSet.SetChangeInfo(CN_ELEMENT_INSERTED, info.id);
		istd::CChangeNotifier changeNotifier(this, &changeSet);

		return info.id;
	}


	return QByteArray();
}


bool CFilterCollectionProxy::GetObjectData(const Id& objectId, DataPtr& dataPtr) const
{
	if (!m_isObjectProxy){
		return BaseClass::GetObjectData(objectId, dataPtr);
	}

	return m_parent.GetObjectData(objectId, dataPtr);
}


bool CFilterCollectionProxy::SetObjectData(
			const Id& objectId,
			const istd::IChangeable& object,
			CompatibilityMode mode,
			const IOperationContext* operationContextPtr)
{
	if (!m_isObjectProxy){
		return BaseClass::SetObjectData(objectId, object, mode, operationContextPtr);
	}

	return m_parent.SetObjectData(objectId, object, mode, operationContextPtr);
}



// reimplemented (istd::IChangeable)

int CFilterCollectionProxy::GetSupportedOperations() const
{
	return SO_CLONE | BaseClass::GetSupportedOperations();
}


istd::IChangeable* CFilterCollectionProxy::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CFilterCollectionProxy> clonePtr(new CFilterCollectionProxy(m_parent, m_isObjectProxy));
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


// protected methods

CFilterCollectionProxy::DataPtr CFilterCollectionProxy::CreateObjectInstance(const QByteArray& typeId) const
{
	istd::IChangeable* objPtr = BaseClass2::CreateInstance(typeId);
	return DataPtr(DataPtr::RootObjectPtr(objPtr), [objPtr]() {
		return objPtr;
	});
}


bool CFilterCollectionProxy::InsertObjectIntoCollection(ObjectInfo info)
{
	imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(info.objectPtr.GetPtr());
	if (modelPtr != nullptr){
		if (!modelPtr->AttachObserver(&m_modelUpdateBridge)){
			qDebug("CObjectCollectionBase::InsertObjectIntoCollection: Attaching object's model to the internal observer failed");

			return false;
		}
	}

	m_objects.push_back(info);

	return true;
}



} // namespace imtbase


