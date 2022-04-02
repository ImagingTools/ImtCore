#include <imtbase/CObjectCollectionComp.h>


namespace imtbase
{


// reimplemented (IObjectCollectionInfo)

const iprm::IOptionsList* CObjectCollectionComp::GetObjectTypesInfo() const
{
	return &m_typesInfo;
}


const ifile::IFilePersistence* CObjectCollectionComp::GetPersistenceForObjectType(const QByteArray& typeId) const
{
	int persistenceIndex = -1;

	for (int i = 0; i < m_typesInfo.GetOptionsCount(); ++i){
		if (typeId == m_typesInfo.GetOptionId(i)){
			persistenceIndex = i;
			break;
		}
	}

	if ((persistenceIndex >= 0) && persistenceIndex < m_objectPersistenceListCompPtr.GetCount()){
		return m_objectPersistenceListCompPtr[persistenceIndex];
	}

	return nullptr;
}


// protected methods

CObjectCollectionComp::DataPtr CObjectCollectionComp::CreateObjectInstance(const QByteArray& typeId) const
{
	int factoryIndex = m_typeIdsAttrPtr.FindValue(typeId);
	if (factoryIndex >= 0){
		if (factoryIndex < m_objectFactoriesCompPtr.GetCount()){
			icomp::IComponent* compPtr = m_objectFactoriesCompPtr.CreateComponent(factoryIndex);
			return DataPtr(DataPtr::RootObjectPtr(compPtr), [compPtr, this]() {
				return m_objectFactoriesCompPtr.ExtractInterface(compPtr);
			});
		}
	}

	return nullptr;
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
			object.flags = OF_ALL & ~OF_SUPPORT_DELETE & ~OF_SUPPORT_PAGINATION;
			object.objectPtr = objectPtr;
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


