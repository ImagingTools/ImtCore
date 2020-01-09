#include <imtbase/CObjectCollection.h>


// Qt includes
#include <QtCore/QUuid>

// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>


namespace imtbase
{


// public methods

CObjectCollection::CObjectCollection()
	:m_modelUpdateBridge(this)
{
}


CObjectCollection::~CObjectCollection()
{
	m_modelUpdateBridge.EnsureModelsDetached();
}


// reimplemented (IObjectCollection)

int CObjectCollection::GetOperationFlags(const QByteArray& objectId) const
{
	if (!objectId.isEmpty()){
		for (const ObjectInfo& objectInfo : m_objects){
			if (objectInfo.id == objectId){
				return objectInfo.flags;
			}
		}
	}

	return OF_SUPPORT_INSERT | OF_SUPPORT_DELETE | OF_SUPPORT_EDIT | OF_SUPPORT_RENAME;
}


bool CObjectCollection::GetDataMetaInfo(const QByteArray& /*objectId*/, MetaInfoPtr& /*metaInfoPtr*/) const
{
	return false;
}


QByteArray CObjectCollection::InsertNewObject(
			const QByteArray& typeId,
			const QString& name,
			const QString& description,
			const istd::IChangeable* defaultValuePtr)
{
	ObjectInfo info;

	info.objectPtr.SetPtr(CreateObjectInstance(typeId), true);
	if (info.objectPtr.IsValid()){
		istd::CChangeNotifier changeNotifier(this);

		if (defaultValuePtr != nullptr){
			if (!info.objectPtr->CopyFrom(*defaultValuePtr)){
				return QByteArray();
			}
		}

		info.description = description;
		info.name = name;
		info.typeId = typeId;
		info.flags = GetItemDefaultFlags();

		if (InsertObjectIntoCollection(info)){
			return info.id;
		}
	}

	return QByteArray();
}


bool CObjectCollection::RemoveObject(const QByteArray& objectId)
{
	for (Objects::iterator iter = m_objects.begin(); iter != m_objects.end(); ++iter){
		if ((*iter).id == objectId){
			imod::IModel* modelPtr = dynamic_cast<imod::IModel*>((*iter).objectPtr.GetPtr());
			if (modelPtr != nullptr){
				modelPtr->DetachObserver(&m_modelUpdateBridge);
			}

			istd::CChangeNotifier changeNotifier(this);

			m_objects.erase(iter);

			break;
		}
	}

	return true;
}


const istd::IChangeable* CObjectCollection::GetObjectPtr(const QByteArray& objectId) const
{
	for (const ObjectInfo& objectInfo : m_objects){
		if ((objectInfo.id == objectId) && ((objectInfo.flags & OF_SUPPORT_DELETE) == 0)){
			return objectInfo.objectPtr.GetPtr();
		}
	}

	return nullptr;
}


bool CObjectCollection::GetObjectData(const QByteArray& objectId, DataPtr& dataPtr) const
{
	for (const ObjectInfo& objectInfo : m_objects){
		if ((objectInfo.id == objectId) && objectInfo.objectPtr.IsValid()){
			if (!dataPtr.IsValid()){
				istd::TDelPtr<istd::IChangeable> newInstancePtr(CreateObjectInstance(objectInfo.typeId));
				if (newInstancePtr.IsValid()){
					if (newInstancePtr->CopyFrom(*objectInfo.objectPtr)){
						dataPtr.SetPtr(newInstancePtr.PopPtr());

						return true;
					}
				}
			}
			else{
				return dataPtr->CopyFrom(*objectInfo.objectPtr);
			}
		}
	}

	return false;
}


bool CObjectCollection::SetObjectData(const QByteArray& objectId, const istd::IChangeable& object)
{
	for (const ObjectInfo& objectInfo : m_objects){
		if ((objectInfo.id == objectId) && objectInfo.objectPtr.IsValid()){
			return objectInfo.objectPtr->CopyFrom(object);
		}
	}

	return false;
}


void CObjectCollection::SetObjectName(const QByteArray& objectId, const QString& objectName)
{
	for (ObjectInfo& objectInfo : m_objects){
		if (objectInfo.id == objectId){
			istd::CChangeNotifier changeNotifier(this);

			objectInfo.name = objectName;
		}
	}
}


void CObjectCollection::SetObjectDescription(const QByteArray& objectId, const QString& objectDescription)
{
	for (ObjectInfo& objectInfo : m_objects){
		if (objectInfo.id == objectId){
			istd::CChangeNotifier changeNotifier(this);

			objectInfo.description = objectDescription;
		}
	}
}


void CObjectCollection::SetObjectEnabled(const QByteArray& objectId, bool isEnabled)
{
	for (ObjectInfo& objectInfo : m_objects){
		if (objectInfo.id == objectId){
			istd::CChangeNotifier changeNotifier(this);

			objectInfo.isEnabled = isEnabled;
		}
	}
}


// reimplemented (IObjectCollectionInfo)

bool CObjectCollection::GetCollectionItemMetaInfo(const QByteArray& /*objectId*/, idoc::IDocumentMetaInfo& /*metaInfo*/) const
{
	return false;
}


const iprm::IOptionsList* CObjectCollection::GetObjectTypesInfo() const
{
	return nullptr;
}


ICollectionInfo::Id CObjectCollection::GetObjectTypeId(const QByteArray& objectId) const
{
	for (const ObjectInfo& objectInfo : m_objects){
		if (objectInfo.id == objectId){
			return objectInfo.typeId;
		}
	}
	
	return QByteArray();
}


// reimplemented (ICollectionInfo)

ICollectionInfo::Ids CObjectCollection::GetElementIds() const
{
	Ids retVal;

	for (const ObjectInfo& objectInfo : m_objects){
		retVal.push_back(objectInfo.id);
	}

	return retVal;
}


QVariant CObjectCollection::GetElementInfo(const QByteArray& elementId, int infoType) const
{
	int objectIndex = -1;
	for (int i = 0; i < m_objects.count(); ++i){
		if (m_objects[i].id == elementId){
			 objectIndex = i;
			 break;
		}
	}

	if (objectIndex >= 0){
		switch (infoType){
		case EIT_DESCRIPTION:
			return m_objects[objectIndex].description;

		case EIT_NAME:
			return m_objects[objectIndex].name;

		case EIT_TYPE_ID:
			return m_objects[objectIndex].typeId;

		case EIT_ENABLED:
			return m_objects[objectIndex].isEnabled;
		}
	}

	return QVariant();
}


// reimplemented (istd::IChangeable)

int CObjectCollection::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CObjectCollection::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CObjectCollection* sourcePtr = dynamic_cast<const CObjectCollection*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		// TODO: Correct this implementation for the fixed objects!
		m_objects.clear();

		for (const ObjectInfo& objectInfo : sourcePtr->m_objects){
			QByteArray newId = InsertNewObject(objectInfo.typeId, objectInfo.name, objectInfo.description, objectInfo.objectPtr.GetPtr());
			if (newId.isEmpty()){
				return false;
			}
		}

		return true;
	}
	
	return false;
}


istd::IChangeable* CObjectCollection::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CObjectCollection> clonePtr(new CObjectCollection());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CObjectCollection::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_objects.clear();

	return true;
}


// protected methods

istd::IChangeable* CObjectCollection::CreateObjectInstance(const QByteArray& typeId) const
{
	return BaseClass::CreateInstance(typeId);
}


bool CObjectCollection::InsertObjectIntoCollection(const ObjectInfo& info)
{
	imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(info.objectPtr.GetPtr());
	if (modelPtr != nullptr){
		modelPtr->AttachObserver(&m_modelUpdateBridge);
	}

	m_objects.push_back(info);

	return true;
}


int CObjectCollection::GetItemDefaultFlags() const
{
	return OF_SUPPORT_DELETE | OF_SUPPORT_RENAME | OF_SUPPORT_EDIT;
}


} // namespace imtbase


