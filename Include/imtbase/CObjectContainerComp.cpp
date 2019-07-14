#include <imtbase/CObjectContainerComp.h>


namespace imtbase
{


// protected methods

istd::IChangeable* CObjectContainerComp::CreateObjectInstance(const QByteArray& typeId) const
{
	int factoryIndex = m_typeIdsAttrPtr.FindValue(typeId);
	if (factoryIndex >= 0){
		if (factoryIndex < m_objectFactoriesCompPtr.GetCount()){
			return m_objectFactoriesCompPtr.CreateInstance(factoryIndex);
		}
	}

	return BaseClass2::CreateInstance(typeId);
}


// reimplemented (icomp::CComponentBase)

void CObjectContainerComp::OnComponentCreated()
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

			QString name = "<unnamed>";
			if (i < m_fixedObjectTypeNamesAttrPtr.GetCount()){
				name = m_fixedObjectTypeNamesAttrPtr[i];
			}

			ObjectInfo object;
			object.isEnabled = true;
			object.name = name;
			object.flags = OF_FIXED;
			object.objectPtr.SetPtr(objectPtr, false);
			object.typeId = typeId;
			object.id = uuid;

			m_objects.push_back(object);
		}
	}
}


} // namespace imtbase


