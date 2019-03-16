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


// reimplemented (IObjectContainer)

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


istd::IChangeable* CObjectContainer::GetEditableObject(const QByteArray& objectId) const
{
	return const_cast<istd::IChangeable*>(GetDataObject(objectId));
}


// reimplemented (IObjectProvider)

const iprm::IOptionsList& CObjectContainer::GetObjectInfoList() const
{
	return *this;
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


QByteArray CObjectContainer::GetObjectTypeId(const QByteArray& objectId) const
{
	for (const ObjectInfo& objectInfo : m_objects){
		if (objectInfo.id == objectId){
			return objectInfo.typeId;
		}
	}
	
	return QByteArray();
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
				istd::IChangeable* newObjectPtr = GetEditableObject(newId);
				Q_ASSERT(newObjectPtr != nullptr);
				if (newObjectPtr != nullptr && objectInfo.object.IsValid()){
					if (!newObjectPtr->CopyFrom(*objectInfo.object.GetPtr())){
						RemoveObject(newId);
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


// protected methods

// reimplemented (IOptionsList)

int CObjectContainer::GetOptionsFlags() const
{
	return SCF_SUPPORT_UNIQUE_ID;
}


int CObjectContainer::GetOptionsCount() const
{
	return m_objects.count();
}


QString CObjectContainer::GetOptionName(int index) const
{
	return m_objects[index].name;
}


QString CObjectContainer::GetOptionDescription(int index) const
{
	return m_objects[index].description;
}


QByteArray CObjectContainer::GetOptionId(int index) const
{
	return m_objects[index].id;
}


bool CObjectContainer::IsOptionEnabled(int /*index*/) const
{
	return true;
}


} // namespace imtbase


