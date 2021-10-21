#include <imtgql/CGqlObject.h>


namespace imtgql
{


// public methods

imtgql::CGqlObject::CGqlObject(const QByteArray& objectId)
	:m_objectId(objectId), m_parentPtr(nullptr)
{
}


QByteArray CGqlObject::GetId() const
{
	return m_objectId;
}


QByteArrayList CGqlObject::GetFieldIds() const
{
	return m_fieldsMap.keys();
}


QVariant CGqlObject::GetFieldArgumentValue(const QByteArray &fieldId) const
{
	QVariant retVal;
	if (m_fieldsMap.contains(fieldId)){
		retVal = m_fieldsMap[fieldId].value;
	}

	return retVal;
}


const CGqlObject* CGqlObject::GetFieldArgumentObjectPtr(const QByteArray &fieldId) const
{
	const CGqlObject* retVal = nullptr;

	if (m_fieldsMap.contains(fieldId)){
		retVal = m_fieldsMap[fieldId].objectPtr.GetPtr();
	}

	return retVal;
}


void CGqlObject::InsertField(const QByteArray &fieldId)
{
	if (!m_fieldsMap.contains(fieldId)){
		Field newField;
		newField.id = fieldId;

		m_fieldsMap[fieldId] = newField;
	}
}


void CGqlObject::InsertFieldArgument(const QByteArray &fieldId, const QVariant &value)
{
	if (m_fieldsMap.contains(fieldId)){
		Field& field = m_fieldsMap[fieldId];
		field.value = value;
	}
}


void CGqlObject::InsertFieldObject(CGqlObject *objectPtr)
{
	if (objectPtr != nullptr){
		QByteArray fieldId = objectPtr->GetId();
		if (!m_fieldsMap.contains(fieldId)){
			Field newField;
			newField.id = fieldId;
			newField.objectPtr.SetPtr(objectPtr);

			m_fieldsMap[fieldId] = newField;
			objectPtr->m_parentPtr = this;
		}
	}
}


bool CGqlObject::IsObject(const QByteArray &fieldId) const
{
	bool retVal = false;
	if (m_fieldsMap.contains(fieldId)){
		if (m_fieldsMap[fieldId].objectPtr.IsValid()){
			retVal = true;
		}
	}
	return retVal;
}


CGqlObject *CGqlObject::GetParentObject() const
{
	return m_parentPtr;
}


} // namespace imtgql


