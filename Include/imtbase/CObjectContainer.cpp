#include <imtbase/CObjectContainer.h>


// Qt includes
#include <QtCore/QUuid>

// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>


namespace imtbase
{


// public methods

CObjectContainer::CObjectContainer()
	:m_modelUpdateBridge(this)
{
}


CObjectContainer::~CObjectContainer()
{
	m_modelUpdateBridge.EnsureModelsDetached();
}


// reimplemented (IObjectCollection)

int CObjectContainer::GetOperationFlags(const QByteArray& objectId) const
{
	if (!objectId.isEmpty()){
		for (const ObjectInfo& objectInfo : m_objects){
			if (objectInfo.id == objectId){
				return objectInfo.flags;
			}
		}
	}

	return OF_DEFAULT;
}


QByteArray CObjectContainer::InsertNewObject(
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

		imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(info.objectPtr.GetPtr());
		if (modelPtr != nullptr){
			modelPtr->AttachObserver(&m_modelUpdateBridge);
		}

		m_objects.push_back(info);

		return info.id;
	}

	return QByteArray();
}


istd::IChangeable* CObjectContainer::GetEditableObject(const QByteArray& objectId) const
{
	for (const ObjectInfo& objectInfo : m_objects){
		if (objectInfo.id == objectId){
			return objectInfo.objectPtr.GetPtr();
		}
	}

	return nullptr;
}


bool CObjectContainer::RemoveObject(const QByteArray& objectId)
{
	for (Objects::iterator iter = m_objects.begin(); iter != m_objects.end(); ++iter){
		if ((*iter).id == objectId){
			imod::IModel* modelPtr = dynamic_cast<imod::IModel*>((*iter).objectPtr.GetPtr());
			if (modelPtr != nullptr){
				modelPtr->DetachObserver(&m_modelUpdateBridge);
			}

			m_objects.erase(iter);

			break;
		}
	}

	return true;
}


void CObjectContainer::SetObjectName(const QByteArray& objectId, const QString& objectName)
{
	for (ObjectInfo& objectInfo : m_objects){
		if (objectInfo.id == objectId){
			istd::CChangeNotifier changeNotifier(this);

			objectInfo.name = objectName;
		}
	}
}


void CObjectContainer::SetObjectDescription(const QByteArray& objectId, const QString& objectDescription)
{
	for (ObjectInfo& objectInfo : m_objects){
		if (objectInfo.id == objectId){
			istd::CChangeNotifier changeNotifier(this);

			objectInfo.description = objectDescription;
		}
	}
}


void CObjectContainer::SetObjectEnabled(const QByteArray& objectId, bool isEnabled)
{
	for (ObjectInfo& objectInfo : m_objects){
		if (objectInfo.id == objectId){
			istd::CChangeNotifier changeNotifier(this);

			objectInfo.isEnabled = isEnabled;
		}
	}
}


// reimplemented (IObjectCollectionInfo)

const iprm::IOptionsList* CObjectContainer::GetObjectTypesInfo() const
{
	return nullptr;
}


ICollectionInfo::Id CObjectContainer::GetObjectTypeId(const QByteArray& objectId) const
{
	for (const ObjectInfo& objectInfo : m_objects){
		if (objectInfo.id == objectId){
			return objectInfo.typeId;
		}
	}
	
	return QByteArray();
}


// reimplemented (IObjectProvider)

const istd::IChangeable* CObjectContainer::GetDataObject(const QByteArray& objectId) const
{
	for (const ObjectInfo& objectInfo : m_objects){
		if (objectInfo.id == objectId){
			return objectInfo.objectPtr.GetPtr();
		}
	}

	return nullptr;
}


// reimplemented (ICollectionInfo)

ICollectionInfo::Ids CObjectContainer::GetElementIds() const
{
	Ids retVal;

	for (const ObjectInfo& objectInfo : m_objects){
		retVal.insert(objectInfo.id);
	}

	return retVal;
}


QString CObjectContainer::GetElementName(const QByteArray & objectId) const
{
	for (const ObjectInfo& objectInfo : m_objects){
		if (objectInfo.id == objectId){
			return objectInfo.name;
		}
	}

	return QString();
}


QString CObjectContainer::GetElementDescription(const QByteArray & objectId) const
{
	for (const ObjectInfo& objectInfo : m_objects){
		if (objectInfo.id == objectId){
			return objectInfo.description;
		}
	}

	return QString();
}


bool CObjectContainer::IsElementEnabled(const QByteArray & objectId) const
{
	for (const ObjectInfo& objectInfo : m_objects){
		if (objectInfo.id == objectId){
			return objectInfo.isEnabled;
		}
	}

	return false;
}


// reimplemented (istd::IChangeable)

int CObjectContainer::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CObjectContainer::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CObjectContainer* sourcePtr = dynamic_cast<const CObjectContainer*>(&object);
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


istd::IChangeable* CObjectContainer::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CObjectContainer> clonePtr(new CObjectContainer());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CObjectContainer::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_objects.clear();

	return true;
}


// protected methods

istd::IChangeable* CObjectContainer::CreateObjectInstance(const QByteArray& typeId) const
{
	return BaseClass::CreateInstance(typeId);
}


} // namespace imtbase


