#include <imtgql/CGqlObject.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>


namespace imtgql
{


// public methods

imtgql::CGqlObject::CGqlObject(const QByteArray& objectId)
{
	m_parentPtr = nullptr;
	m_objectId = objectId;
}


QByteArray CGqlObject::GetId() const
{
	return m_objectId;
}


QByteArrayList CGqlObject::GetFieldIds() const
{
	QByteArrayList retVal = m_emptyFields;
	retVal += m_simpleFields.keys();
	retVal += m_objectFields.keys();
	retVal += m_objectFieldsArray.keys();

	return retVal;
}


QVariant CGqlObject::GetFieldArgumentValue(const QByteArray &fieldId) const
{
	QVariant retVal;
	if (m_simpleFields.contains(fieldId)){
		retVal = m_simpleFields[fieldId];
	}

	return retVal;
}


CGqlObject *CGqlObject::CreateFieldObject(const QByteArray& fieldId)
{
	if (m_objectFields.contains(fieldId)){
		return nullptr;
	}

	CGqlObject gqlObject(fieldId);
	InsertField(fieldId, gqlObject);

	return m_objectFields[fieldId].GetPtr();
}


const CGqlObject* CGqlObject::GetFieldArgumentObjectPtr(const QByteArray &fieldId,int index) const
{
	const CGqlObject* retVal = nullptr;

	if (m_objectFields.contains(fieldId)){
		retVal = m_objectFields[fieldId].GetPtr();
	}
	else if (m_objectFieldsArray.contains(fieldId)){
		retVal = m_objectFieldsArray[fieldId][index].GetPtr();
	}

	return retVal;
}


QList<const CGqlObject *> CGqlObject::GetFieldArgumentObjectPtrList(const QByteArray &fieldId) const
{
	QList<const CGqlObject*> retVal;
	if (m_objectFieldsArray.contains(fieldId)){
		for (int i = 0; i < m_objectFieldsArray[fieldId].count(); i++){
			const CGqlObject* gqlObject = m_objectFieldsArray[fieldId][i].GetPtr();
			retVal.append(gqlObject);
		}
	}

	return retVal;
}


void CGqlObject::InsertField(const QByteArray &fieldId)
{
	if (!m_emptyFields.contains(fieldId)){
		m_emptyFields.append(fieldId);
	}
}


void CGqlObject::InsertField(const QByteArray &fieldId, const QVariant &value)
{
	RemoveField(fieldId);
	m_simpleFields.insert(fieldId, value);
}


void CGqlObject::InsertField(const QByteArray &fieldId, const CGqlEnum &value)
{
	RemoveField(fieldId);
	m_simpleFields.insert(fieldId, value);
}


void CGqlObject::InsertField(const QByteArray &fieldId, const CGqlObject& object)
{
	istd::TSmartPtr<CGqlObject> objectPtr(new CGqlObject());
	*objectPtr = object;
	objectPtr->m_parentPtr = this;
	QByteArray fieldIdLocal = fieldId;
	if (fieldId.isEmpty()){
		fieldIdLocal = objectPtr->GetId();
	}
	RemoveField(fieldIdLocal);
	m_objectFields.insert(fieldIdLocal, objectPtr);
}


void CGqlObject::InsertField(const QByteArray &fieldId, const QList<CGqlObject> objectList)
{
	QList<istd::TSmartPtr<CGqlObject>> objectPtrList;
	for (int i = 0; i < objectList.count(); i++){
		istd::TSmartPtr<CGqlObject> objectPtr(new CGqlObject());
		*objectPtr = objectList[i];
		objectPtr->m_parentPtr = this;
		objectPtrList.append(objectPtr);
	}
	RemoveField(fieldId);
	m_objectFieldsArray.insert(fieldId,objectPtrList);
}


bool CGqlObject::IsObject(const QByteArray &fieldId) const
{
	bool retVal = false;
	if (m_objectFields.contains(fieldId)){
		retVal = true;
	}

	return retVal;
}


bool CGqlObject::IsObjectList(const QByteArray &fieldId) const
{
	bool retVal = false;
	if (m_objectFieldsArray.contains(fieldId)){
		retVal = true;
	}

	return retVal;
}


bool CGqlObject::IsEnum(const QByteArray &fieldId) const
{
	bool retVal = false;

	if (m_simpleFields.contains(fieldId)){
		if (m_simpleFields[fieldId].canConvert<CGqlEnum>()){
			retVal = true;
		}
	}

	return retVal;
}

int CGqlObject::GetObjectsCount(const QByteArray &fieldId) const
{
	int retVal = 0;
	if (m_objectFieldsArray.contains(fieldId)){
		retVal = m_objectFieldsArray[fieldId].count();
	}

	return retVal;
}


CGqlObject *CGqlObject::GetParentObject() const
{
	return m_parentPtr;
}


// reimplemented (iser::ISerializable)

bool CGqlObject::Serialize(iser::IArchive &archive)
{
	return true;
}


// reimplemented (istd::IChangeable)

bool CGqlObject::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CGqlObject* sourcePtr = dynamic_cast<const CGqlObject*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_emptyFields = sourcePtr->m_emptyFields;
		m_simpleFields = sourcePtr->m_simpleFields;
		m_objectId = sourcePtr->m_objectId;
		m_parentPtr = sourcePtr->m_parentPtr;

		m_objectFields.clear();

		QByteArrayList keys = sourcePtr->m_objectFields.keys();
		for (const QByteArray& key : keys){
			istd::TSmartPtr<CGqlObject> sourceObject = sourcePtr->m_objectFields.value(key);

			istd::TSmartPtr<CGqlObject> object;
			object.SetCastedOrRemove(sourceObject->CloneMe());
			if (!object.IsValid()){
				return false;
			}

			m_objectFields.insert(key, object);
		}

		m_objectFieldsArray.clear();

		QByteArrayList objectFieldsArrayKeys = sourcePtr->m_objectFieldsArray.keys();
		for (const QByteArray& key : objectFieldsArrayKeys){
			QList<istd::TSmartPtr<CGqlObject>> value;
			QList<istd::TSmartPtr<CGqlObject>> sourceValue = sourcePtr->m_objectFieldsArray.value(key);
			for (const istd::TSmartPtr<CGqlObject>& sourceObject : sourceValue){
				istd::TSmartPtr<CGqlObject> emptyObject;
				emptyObject.SetCastedOrRemove(sourceObject->CloneMe());
				if (!emptyObject.IsValid()){
					return false;
				}

				value.append(emptyObject);
			}

			m_objectFieldsArray.insert(key, value);
		}

		return true;
	}

	return false;
}


istd::IChangeable* CGqlObject::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CGqlObject> clonePtr(new CGqlObject);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CGqlObject::ResetData(CompatibilityMode mode)
{
	istd::CChangeNotifier changeNotifier(this);

	m_emptyFields.clear();
	m_simpleFields.clear();
	m_objectId.clear();
	m_parentPtr = nullptr;
	m_objectFields.clear();
	m_objectFieldsArray.clear();

	return true;
}


// protected methods

void CGqlObject::RemoveField(const QByteArray &fieldId)
{
	m_emptyFields.removeAll(fieldId);
	m_simpleFields.remove(fieldId);
	m_objectFields.remove(fieldId);
	m_objectFieldsArray.remove(fieldId);
}


} // namespace imtgql


