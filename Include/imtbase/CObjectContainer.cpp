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


// reimplemented (IObjectManager)

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
			const QString& description)
{
	ObjectInfo info;

	info.object.SetPtr(CreateObjectInstance(typeId));
	if (info.object.IsValid()){
		istd::CChangeNotifier changeNotifier(this);

		info.description = description;
		info.name = name;
		info.typeId = typeId;

		imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(info.object.GetPtr());
		if (modelPtr != nullptr){
			modelPtr->AttachObserver(&m_modelUpdateBridge);
		}

		m_objects.push_back(info);

		return info.id;
	}

	return QByteArray();
}


CObjectContainer::ObjectPtr CObjectContainer::GetEditableObject(const QByteArray& objectId)
{
	for (const ObjectInfo& objectInfo : m_objects){
		if (objectInfo.id == objectId){
			return objectInfo.object;
		}
	}

	return ObjectPtr();
}


bool CObjectContainer::RemoveObject(const QByteArray& objectId)
{
	for (Objects::iterator iter = m_objects.begin(); iter != m_objects.end(); ++iter){
		if ((*iter).id == objectId){
			imod::IModel* modelPtr = dynamic_cast<imod::IModel*>((*iter).object.GetPtr());
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


// reimplemented (IObjectProvider)

const iprm::IOptionsList* CObjectContainer::GetSupportedObjectTypes() const
{
	return nullptr;
}


IElementList::Id CObjectContainer::GetObjectTypeId(const QByteArray& objectId) const
{
	for (const ObjectInfo& objectInfo : m_objects){
		if (objectInfo.id == objectId){
			return objectInfo.typeId;
		}
	}
	
	return QByteArray();
}


const istd::IChangeable* CObjectContainer::GetDataObject(const QByteArray& objectId) const
{
	for (const ObjectInfo& objectInfo : m_objects){
		if (objectInfo.id == objectId){
			return objectInfo.object.GetPtr();
		}
	}

	return nullptr;
}


// reimplemented (IElementList)

IElementList::Ids CObjectContainer::GetElementIds() const
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

		m_objects.clear();

		for (const ObjectInfo& objectInfo : sourcePtr->m_objects){
			QByteArray newId = InsertNewObject(objectInfo.typeId, objectInfo.name, objectInfo.description);
			if (!newId.isEmpty()){
				ObjectPtr newObjectPtr = GetEditableObject(newId);
				Q_ASSERT(newObjectPtr.IsValid());
				if (newObjectPtr.IsValid() && objectInfo.object.IsValid()){
					if (!newObjectPtr->CopyFrom(*objectInfo.object.GetPtr())){
						RemoveObject(newId);

						return false;
					}
				}
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


