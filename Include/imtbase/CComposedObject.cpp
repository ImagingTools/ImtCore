#include <imtbase/CComposedObject.h>


// Qt includes
#include <QtCore/QUuid>


namespace imtbase
{


// public methods

CComposedObject::CComposedObject()
	:m_modelUpdateBridge(this)
{
}


CComposedObject::~CComposedObject()
{
	m_modelUpdateBridge.EnsureModelsDetached();
}


// reimplemented (IObjectContainer)

QByteArray CComposedObject::InsertNewObject(
			const QByteArray& typeId,
			const QString& name,
			const QString& description)
{
	ObjectInfo info;

	info.object.SetPtr(CreateObjectInstance(typeId));
	if (info.object.IsValid()){
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


bool CComposedObject::RemoveObject(const QByteArray& objectId)
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


// reimplemented (IObjectProvider)

const iprm::IOptionsList& CComposedObject::GetObjectInfoList() const
{
	return *this;
}


const istd::IChangeable* CComposedObject::GetObject(const QByteArray& objectId) const
{
	for (const ObjectInfo& objectInfo : m_objects){
		if (objectInfo.id == objectId){
			return objectInfo.object.GetPtr();
		}
	}

	return nullptr;
}


QByteArray CComposedObject::GetObjectTypeId(const QByteArray& objectId) const
{
	for (const ObjectInfo& objectInfo : m_objects){
		if (objectInfo.id == objectId){
			return objectInfo.typeId;
		}
	}
	
	return QByteArray();
}


// protected methods

istd::IChangeable* CComposedObject::CreateObjectInstance(const QByteArray& typeId) const
{
	return BaseClass::CreateInstance(typeId);
}


// protected methods

// reimplemented (IOptionsList)

int CComposedObject::GetOptionsFlags() const
{
	return SCF_SUPPORT_UNIQUE_ID;
}


int CComposedObject::GetOptionsCount() const
{
	return m_objects.count();
}


QString CComposedObject::GetOptionName(int index) const
{
	return m_objects[index].name;
}


QString CComposedObject::GetOptionDescription(int index) const
{
	return m_objects[index].description;
}


QByteArray CComposedObject::GetOptionId(int index) const
{
	return m_objects[index].id;
}


bool CComposedObject::IsOptionEnabled(int /*index*/) const
{
	return true;
}


} // namespace imtbase


