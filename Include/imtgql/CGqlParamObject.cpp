// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtgql/CGqlParamObject.h>


// ACF includes
#include <istd/CChangeNotifier.h>


namespace imtgql
{


// public methods

CGqlParamObject::CGqlParamObject(): m_parentPtr(nullptr)
{
}


QByteArrayList CGqlParamObject::GetParamIds() const
{
	QByteArrayList retVal = m_simpleParams.keys();
	retVal += m_objectParams.keys();
	retVal += m_objectParamsArray.keys();

	return retVal;
}


QVariant CGqlParamObject::GetParamArgumentValue(const QByteArray &paramId) const
{
	QVariant retVal;
	if (m_simpleParams.contains(paramId)){
		retVal = m_simpleParams[paramId];
	}
	else if (m_objectParamsArray.contains(paramId)){
		QVariantList objectList;
		for (int i = 0; i < m_objectParamsArray[paramId].count(); i++){
			const CGqlParamObject* gqlObject = m_objectParamsArray[paramId][i].GetPtr();
			objectList.append(QVariant::fromValue(gqlObject));
		}
		retVal = objectList;
	}

	return retVal;
}


CGqlParamObject *CGqlParamObject::CreateParamObject(const QByteArray& paramId)
{
	if (m_objectParams.contains(paramId)){
		return nullptr;
	}

	CGqlParamObject gqlObject;
	InsertParam(paramId, gqlObject);

	return m_objectParams[paramId].GetPtr();
}


const CGqlParamObject* CGqlParamObject::GetParamArgumentObjectPtr(const QByteArray &paramId, qsizetype index) const
{
	const CGqlParamObject* retVal = nullptr;

	if (m_objectParams.contains(paramId)){
		retVal = m_objectParams[paramId].GetPtr();
	}
	else if (m_objectParamsArray.contains(paramId)){
		retVal = m_objectParamsArray[paramId][index].GetPtr();
	}

	return retVal;
}


QList<const CGqlParamObject *> CGqlParamObject::GetParamArgumentObjectPtrList(const QByteArray &paramId) const
{
	QList<const CGqlParamObject*> retVal;
	if (m_objectParamsArray.contains(paramId)){
		for (int i = 0; i < m_objectParamsArray[paramId].count(); i++){
			const CGqlParamObject* gqlObject = m_objectParamsArray[paramId][i].GetPtr();
			retVal.append(gqlObject);
		}
	}

	return retVal;
}


void CGqlParamObject::InsertParam(const QByteArray &paramId, const QVariant &value)
{
	RemoveParam(paramId);
	m_simpleParams.insert(paramId, value);
}


void CGqlParamObject::InsertParam(const QByteArray &paramId, const CGqlEnum &value)
{
	RemoveParam(paramId);
	m_simpleParams.insert(paramId, value);
}


void CGqlParamObject::InsertParam(const QByteArray &paramId, const CGqlParamObject& object)
{
	istd::TSharedInterfacePtr<CGqlParamObject> objectPtr(new CGqlParamObject());
	*objectPtr = object;
	objectPtr->m_parentPtr = this;
	RemoveParam(paramId);
	m_objectParams.insert(paramId, objectPtr);
}


void CGqlParamObject::InsertParam(const QByteArray &paramId, const QList<CGqlParamObject>& objectList)
{
	QList<istd::TSharedInterfacePtr<CGqlParamObject>> objectPtrList;
	for (int i = 0; i < objectList.count(); i++){
		istd::TSharedInterfacePtr<CGqlParamObject> objectPtr(new CGqlParamObject());
		*objectPtr = objectList[i];
		objectPtr->m_parentPtr = this;
		objectPtrList.append(objectPtr);
	}
	RemoveParam(paramId);
	m_objectParamsArray.insert(paramId,objectPtrList);
}


CGqlParamObject* CGqlParamObject::AppendParamToArray(const QByteArray& paramId, const CGqlParamObject& object)
{
	CGqlParamObject* retVal = nullptr;
	if (!m_objectParamsArray.contains(paramId)){
		QList<istd::TSharedInterfacePtr<CGqlParamObject>> emptyList;
		m_objectParamsArray.insert(paramId, emptyList);
	}

	istd::TSharedInterfacePtr<CGqlParamObject> objectPtr(new CGqlParamObject());
	*objectPtr = object;
	objectPtr->m_parentPtr = this;
	m_objectParamsArray[paramId].append(objectPtr);
	retVal = objectPtr.GetPtr();

	return retVal;
}


bool CGqlParamObject::IsObject(const QByteArray &paramId) const
{
	bool retVal = false;
	if (m_objectParams.contains(paramId)){
		retVal = true;
	}

	return retVal;
}


bool CGqlParamObject::IsObjectList(const QByteArray &paramId) const
{
	bool retVal = false;
	if (m_objectParamsArray.contains(paramId)){
		retVal = true;
	}

	return retVal;
}


bool CGqlParamObject::IsEnum(const QByteArray &paramId) const
{
	bool retVal = false;

	if (m_simpleParams.contains(paramId)){
		if (m_simpleParams[paramId].canConvert<CGqlEnum>()){
			retVal = true;
		}
	}

	return retVal;
}


qsizetype CGqlParamObject::GetObjectsCount(const QByteArray &paramId) const
{
	qsizetype retVal = 0;
	if (m_objectParamsArray.contains(paramId)){
		retVal = m_objectParamsArray[paramId].size();
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

		m_simpleParams = sourcePtr->m_simpleParams;
		m_objectId = sourcePtr->m_objectId;
		m_parentPtr = sourcePtr->m_parentPtr;

		m_objectParams.clear();

		QByteArrayList keys = sourcePtr->m_objectParams.keys();
		for (const QByteArray& key : keys){
			istd::TSharedInterfacePtr<CGqlParamObject> sourceObject = sourcePtr->m_objectParams.value(key);

			istd::TSharedInterfacePtr<CGqlParamObject> gqlObject;
			gqlObject.MoveCastedPtr(sourceObject->CloneMe());
			if (!gqlObject.IsValid()){
				return false;
			}

			m_objectParams.insert(key, gqlObject);
		}

		m_objectParamsArray.clear();

		QByteArrayList objectParamsArrayKeys = sourcePtr->m_objectParamsArray.keys();
		for (const QByteArray& key : objectParamsArrayKeys){
			QList<istd::TSharedInterfacePtr<CGqlParamObject>> value;
			QList<istd::TSharedInterfacePtr<CGqlParamObject>> sourceValue = sourcePtr->m_objectParamsArray.value(key);
			for (const istd::TSharedInterfacePtr<CGqlParamObject>& sourceObject : sourceValue){
				istd::TSharedInterfacePtr<CGqlParamObject> emptyObject;
				emptyObject.MoveCastedPtr(sourceObject->CloneMe());
				if (!emptyObject.IsValid()){
					return false;
				}

				value.append(emptyObject);
			}

			m_objectParamsArray.insert(key, value);
		}

		return true;
	}

	return false;
}


istd::IChangeableUniquePtr CGqlParamObject::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CGqlParamObject);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CGqlParamObject::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_simpleParams.clear();
	m_objectId.clear();
	m_parentPtr = nullptr;
	m_objectParams.clear();
	m_objectParamsArray.clear();

	return true;
}


// protected methods

void CGqlParamObject::RemoveParam(const QByteArray &paramId)
{
	m_simpleParams.remove(paramId);
	m_objectParams.remove(paramId);
	m_objectParamsArray.remove(paramId);
}


} // namespace imtgql


