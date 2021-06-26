#include <imtgql/CGqlObject.h>


namespace imtgql
{


// public methods

imtgql::CGqlObject::CGqlObject(const QByteArray& objectId)
	: m_objectId(objectId)
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
	else{
		Q_ASSERT(false);
	}
	return retVal;
}


const CGqlObject* CGqlObject::GetFieldArgumentObjectPtr(const QByteArray &fieldId) const
{
	const CGqlObject* retVal = nullptr;
	if (m_fieldsMap.contains(fieldId)){
		retVal = m_fieldsMap[fieldId].objectPtr.GetPtr();
	}
	else{
		Q_ASSERT(false);
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
	else{
		Q_ASSERT(false);
	}
}


void CGqlObject::InsertFieldArgument(const QByteArray &fieldId, const QVariant &value)
{
	if (m_fieldsMap.contains(fieldId)){
		Field& field = m_fieldsMap[fieldId];
		field.value = value;
	}
	else{
		Q_ASSERT(false);
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
		}
		else{
			Q_ASSERT(false);
		}
	}
}


bool CGqlObject::isObject(const QByteArray &fieldId) const
{
	bool retVal = false;
	if (m_fieldsMap.contains(fieldId)){
		if (m_fieldsMap[fieldId].objectPtr.IsValid()){
			retVal = true;
		}
	}
	return retVal;
}


} // namespace imtgql


