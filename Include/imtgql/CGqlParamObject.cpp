#include <imtgql/CGqlParamObject.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>


namespace imtgql
{


// public methods

CGqlParamObject::CGqlParamObject(): m_parentPtr(nullptr)
{
}


QByteArrayList CGqlParamObject::GetFieldIds() const
{
	QByteArrayList retVal = m_simpleFields.keys();
	retVal += m_objectFields.keys();
	retVal += m_objectFieldsArray.keys();

	return retVal;
}


QVariant CGqlParamObject::GetFieldArgumentValue(const QByteArray &fieldId) const
{
	QVariant retVal;
	if (m_simpleFields.contains(fieldId)){
		retVal = m_simpleFields[fieldId];
	}
	else if (m_objectFieldsArray.contains(fieldId)){
		QVariantList objectList;
		for (int i = 0; i < m_objectFieldsArray[fieldId].count(); i++){
			const CGqlParamObject* gqlObject = m_objectFieldsArray[fieldId][i].GetPtr();
			objectList.append(QVariant::fromValue(gqlObject));
		}
		retVal = objectList;
	}

	return retVal;
}


CGqlParamObject *CGqlParamObject::CreateFieldObject(const QByteArray& fieldId)
{
	if (m_objectFields.contains(fieldId)){
		return nullptr;
	}

	CGqlParamObject gqlObject;
	InsertField(fieldId, gqlObject);

	return m_objectFields[fieldId].GetPtr();
}


const CGqlParamObject* CGqlParamObject::GetFieldArgumentObjectPtr(const QByteArray &fieldId,int index) const
{
	const CGqlParamObject* retVal = nullptr;

	if (m_objectFields.contains(fieldId)){
		retVal = m_objectFields[fieldId].GetPtr();
	}
	else if (m_objectFieldsArray.contains(fieldId)){
		retVal = m_objectFieldsArray[fieldId][index].GetPtr();
	}

	return retVal;
}


QList<const CGqlParamObject *> CGqlParamObject::GetFieldArgumentObjectPtrList(const QByteArray &fieldId) const
{
	QList<const CGqlParamObject*> retVal;
	if (m_objectFieldsArray.contains(fieldId)){
		for (int i = 0; i < m_objectFieldsArray[fieldId].count(); i++){
			const CGqlParamObject* gqlObject = m_objectFieldsArray[fieldId][i].GetPtr();
			retVal.append(gqlObject);
		}
	}

	return retVal;
}


void CGqlParamObject::InsertField(const QByteArray &fieldId, const QVariant &value)
{
	RemoveField(fieldId);
	m_simpleFields.insert(fieldId, value);
}


void CGqlParamObject::InsertField(const QByteArray &fieldId, const CGqlEnum &value)
{
	RemoveField(fieldId);
	m_simpleFields.insert(fieldId, value);
}


void CGqlParamObject::InsertField(const QByteArray &fieldId, const CGqlParamObject& object)
{
	istd::TSmartPtr<CGqlParamObject> objectPtr(new CGqlParamObject());
	*objectPtr = object;
	objectPtr->m_parentPtr = this;
	RemoveField(fieldId);
	m_objectFields.insert(fieldId, objectPtr);
}


void CGqlParamObject::InsertField(const QByteArray &fieldId, const QList<CGqlParamObject> objectList)
{
	QList<istd::TSmartPtr<CGqlParamObject>> objectPtrList;
	for (int i = 0; i < objectList.count(); i++){
		istd::TSmartPtr<CGqlParamObject> objectPtr(new CGqlParamObject());
		*objectPtr = objectList[i];
		objectPtr->m_parentPtr = this;
		objectPtrList.append(objectPtr);
	}
	RemoveField(fieldId);
	m_objectFieldsArray.insert(fieldId,objectPtrList);
}


CGqlParamObject* CGqlParamObject::AppendFieldToArray(const QByteArray& fieldId, const CGqlParamObject& object)
{
	CGqlParamObject* retVal = nullptr;
	if (!m_objectFieldsArray.contains(fieldId)){
		QList<istd::TSmartPtr<CGqlParamObject>> emptyList;
		m_objectFieldsArray.insert(fieldId, emptyList);
	}

	istd::TSmartPtr<CGqlParamObject> objectPtr(new CGqlParamObject());
	*objectPtr = object;
	objectPtr->m_parentPtr = this;
	m_objectFieldsArray[fieldId].append(objectPtr);
	retVal = objectPtr.GetPtr();

	return retVal;
}


bool CGqlParamObject::IsObject(const QByteArray &fieldId) const
{
	bool retVal = false;
	if (m_objectFields.contains(fieldId)){
		retVal = true;
	}

	return retVal;
}


bool CGqlParamObject::IsObjectList(const QByteArray &fieldId) const
{
	bool retVal = false;
	if (m_objectFieldsArray.contains(fieldId)){
		retVal = true;
	}

	return retVal;
}


bool CGqlParamObject::IsEnum(const QByteArray &fieldId) const
{
	bool retVal = false;

	if (m_simpleFields.contains(fieldId)){
		if (m_simpleFields[fieldId].canConvert<CGqlEnum>()){
			retVal = true;
		}
	}

	return retVal;
}


int CGqlParamObject::GetObjectsCount(const QByteArray &fieldId) const
{
	int retVal = 0;
	if (m_objectFieldsArray.contains(fieldId)){
		retVal = m_objectFieldsArray[fieldId].count();
	}

	return retVal;
}


CGqlParamObject *CGqlParamObject::GetParentObject() const
{
	return m_parentPtr;
}


// reimplemented (iser::ISerializable)

bool CGqlParamObject::Serialize(iser::IArchive& /*archive*/)
{
	return true;
}


// reimplemented (istd::IChangeable)

bool CGqlParamObject::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CGqlParamObject* sourcePtr = dynamic_cast<const CGqlParamObject*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_simpleFields = sourcePtr->m_simpleFields;
		m_objectId = sourcePtr->m_objectId;
		m_parentPtr = sourcePtr->m_parentPtr;

		m_objectFields.clear();

		QByteArrayList keys = sourcePtr->m_objectFields.keys();
		for (const QByteArray& key : keys){
			istd::TSmartPtr<CGqlParamObject> sourceObject = sourcePtr->m_objectFields.value(key);

			istd::TSmartPtr<CGqlParamObject> gqlObject;
			gqlObject.SetCastedOrRemove(sourceObject->CloneMe());
			if (!gqlObject.IsValid()){
				return false;
			}

			m_objectFields.insert(key, gqlObject);
		}

		m_objectFieldsArray.clear();

		QByteArrayList objectFieldsArrayKeys = sourcePtr->m_objectFieldsArray.keys();
		for (const QByteArray& key : objectFieldsArrayKeys){
			QList<istd::TSmartPtr<CGqlParamObject>> value;
			QList<istd::TSmartPtr<CGqlParamObject>> sourceValue = sourcePtr->m_objectFieldsArray.value(key);
			for (const istd::TSmartPtr<CGqlParamObject>& sourceObject : sourceValue){
				istd::TSmartPtr<CGqlParamObject> emptyObject;
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


istd::IChangeable* CGqlParamObject::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CGqlParamObject> clonePtr(new CGqlParamObject);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CGqlParamObject::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_simpleFields.clear();
	m_objectId.clear();
	m_parentPtr = nullptr;
	m_objectFields.clear();
	m_objectFieldsArray.clear();

	return true;
}


// protected methods

void CGqlParamObject::RemoveField(const QByteArray &fieldId)
{
	m_simpleFields.remove(fieldId);
	m_objectFields.remove(fieldId);
	m_objectFieldsArray.remove(fieldId);
}


} // namespace imtgql


