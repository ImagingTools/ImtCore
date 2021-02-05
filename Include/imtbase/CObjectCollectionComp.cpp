#include <imtbase/CObjectCollectionComp.h>


namespace imtbase
{


// reimplemented (IObjectCollectionInfo)

const iprm::IOptionsList* CObjectCollectionComp::GetObjectTypesInfo() const
{
	return &m_typesInfo;
}


// protected methods

istd::IChangeable* CObjectCollectionComp::CreateObjectInstance(const QByteArray& typeId) const
{
	int factoryIndex = m_typeIdsAttrPtr.FindValue(typeId);
	if (factoryIndex >= 0){
		if (factoryIndex < m_objectFactoriesCompPtr.GetCount()){
			return m_objectFactoriesCompPtr.CreateInstance(factoryIndex);
		}
	}

	return nullptr;
}


void CObjectCollectionComp::DestroyObjectInstance(istd::IChangeable* objectPtr) const
{
	if (objectPtr != nullptr){
		icomp::IComponent* componentPtr = dynamic_cast<icomp::IComponent*>(objectPtr);
		if (componentPtr != nullptr){
			const icomp::ICompositeComponent* parentComponentPtr = nullptr;
			while ((parentComponentPtr = componentPtr->GetParentComponent(true)) != nullptr){
				componentPtr = const_cast<icomp::ICompositeComponent*>(parentComponentPtr);
			}

			if (componentPtr != nullptr){
				delete componentPtr;
			}
		}
		else{
			delete objectPtr;
		}
	}
}


// reimplemented (icomp::CComponentBase)

void CObjectCollectionComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	int fixedObjectCount = qMin(qMin(m_fixedObjectsCompPtr.GetCount(), m_fixedObjectIdsAttrPtr.GetCount()), m_fixedObjectTypeIdsAttrPtr.GetCount());

	for (int i = 0; i < fixedObjectCount; ++i){
		istd::IChangeable* objectPtr = m_fixedObjectsCompPtr[i];
		if (objectPtr != nullptr){
			QByteArray uuid = m_fixedObjectIdsAttrPtr[i];
			Q_ASSERT(!uuid.isEmpty());
			if (uuid.isEmpty()){
				continue;
			}

			QByteArray typeId = m_fixedObjectTypeIdsAttrPtr[i];
			Q_ASSERT(!typeId.isEmpty());
			if (typeId.isEmpty()){
				continue;
			}

			QString typeName;
			if (i < m_fixedObjectTypeNamesAttrPtr.GetCount()){
				typeName = m_fixedObjectTypeNamesAttrPtr[i];
			}

			QString objectName = "<unnamed>";
			if (i < m_fixedObjectNamesAttrPtr.GetCount()){
				objectName = m_fixedObjectNamesAttrPtr[i];
			}

			ObjectInfo object;
			object.isEnabled = true;
			object.name = objectName;
			object.flags = OF_ALL & ~OF_SUPPORT_DELETE;
			object.objectPtr.SetPtr(objectPtr, false);
			object.typeId = typeId;
			object.id = uuid;

			InsertObjectIntoCollection(object);

			m_typesInfo.InsertOption(typeName, typeId);
		}
	}

	for (int i = 0; i < qMin(m_typeIdsAttrPtr.GetCount(), m_typeNamesAttrPtr.GetCount()); ++i){
		QByteArray typeId = m_typeIdsAttrPtr[i];

		int foundIndex = m_typesInfo.GetOptionIndexById(typeId);
		if (foundIndex < 0){
			m_typesInfo.InsertOption(m_typeNamesAttrPtr[i], typeId);
		}
	}
}


void CObjectCollectionComp::OnComponentDestroyed()
{
	RemoveAllObjects();

	BaseClass::OnComponentDestroyed();
}


} // namespace imtbase


