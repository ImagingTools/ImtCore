// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtgql/CGqlFieldObject.h>


// ACF includes
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtgql/CGqlFieldFragment.h>


namespace imtgql
{


// public methods

CGqlFieldObject::CGqlFieldObject(): m_parentPtr(nullptr)
{
}


QByteArrayList CGqlFieldObject::GetFieldIds() const
{
	QByteArrayList retVal = m_emptyFields;
	retVal += m_objectFields.keys();
	retVal += m_fragmentFields.keys();

	return retVal;
}


CGqlFieldObject *CGqlFieldObject::CreateFieldObject(const QByteArray& fieldId)
{
	if (m_objectFields.contains(fieldId)){
		return nullptr;
	}

	CGqlFieldObject gqlObject;
	InsertField(fieldId, gqlObject);

	return m_objectFields[fieldId].get();
}


CGqlFieldFragment* CGqlFieldObject::CreateFragmentObject(const QByteArray& typeId)
{
	if (m_fragmentFields.contains(typeId)){
		return nullptr;
	}

	CGqlFieldFragment gqlFragment;
	InsertFragment(typeId, gqlFragment);

	return m_fragmentFields[typeId].get();
}


void CGqlFieldObject::InsertField(const QByteArray &fieldId)
{
	if (!m_emptyFields.contains(fieldId)){
		m_emptyFields.append(fieldId);
	}
}


void CGqlFieldObject::InsertField(const QByteArray &fieldId, const CGqlFieldObject& object)
{
	std::shared_ptr<CGqlFieldObject> objectPtr(new CGqlFieldObject());
	*objectPtr = object;
	objectPtr->m_parentPtr = this;

	RemoveField(fieldId);

	m_objectFields.insert(fieldId, objectPtr);
}


void CGqlFieldObject::InsertFragment(const QByteArray& typeId, const CGqlFieldFragment& object)
{
	std::shared_ptr<CGqlFieldFragment> fragmentPtr(new CGqlFieldFragment());
	*fragmentPtr = object;
	fragmentPtr->m_parentPtr = this;

	RemoveField(typeId);

	m_fragmentFields.insert(typeId,fragmentPtr);
}


bool CGqlFieldObject::IsObject(const QByteArray &fieldId) const
{
	bool retVal = false;
	if (m_objectFields.contains(fieldId)){
		retVal = true;
	}

	return retVal;
}


bool CGqlFieldObject::IsFragment(const QByteArray& fieldId) const
{
	bool retVal = false;
	if (m_fragmentFields.contains(fieldId)){
		retVal = true;
	}

	return retVal;
}


const CGqlFieldObject* CGqlFieldObject::GetFieldArgumentObjectPtr(const QByteArray& fieldId) const
{
	const CGqlFieldObject* retVal = nullptr;

	if (m_objectFields.contains(fieldId)){
		retVal = m_objectFields[fieldId].get();
	}

	return retVal;
}


const CGqlFieldFragment* CGqlFieldObject::GetFragmentArgumentObjectPtr(const QByteArray& fieldId) const
{
	const CGqlFieldFragment* retVal = nullptr;

	if (m_fragmentFields.contains(fieldId)){
		retVal = m_fragmentFields[fieldId].get();
	}

	return retVal;
}


CGqlFieldObject* CGqlFieldObject::GetParentObject() const
{
	return m_parentPtr;
}


// reimplemented (iser::ISerializable)

bool CGqlFieldObject::Serialize(iser::IArchive& /*archive*/)
{
	return true;
}


// reimplemented (istd::IChangeable)

bool CGqlFieldObject::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CGqlFieldObject* sourcePtr = dynamic_cast<const CGqlFieldObject*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_emptyFields = sourcePtr->m_emptyFields;
		m_parentPtr = sourcePtr->m_parentPtr;

		m_objectFields.clear();

		QByteArrayList keys = sourcePtr->m_objectFields.keys();
		for (const QByteArray& key : keys){
			std::shared_ptr<CGqlFieldObject> sourceObject = sourcePtr->m_objectFields.value(key);

			std::shared_ptr<CGqlFieldObject> gqlObject;
			std::shared_ptr<istd::IChangeable> clonedPtr(sourceObject->CloneMe().PopInterfacePtr());
			Q_ASSERT(clonedPtr != nullptr);

			gqlObject = std::dynamic_pointer_cast<CGqlFieldObject>(clonedPtr);
			if (gqlObject == nullptr){
				Q_ASSERT(false);

				return false;
			}

			m_objectFields.insert(key, gqlObject);
		}

		m_fragmentFields.clear();

		keys = sourcePtr->m_fragmentFields.keys();
		for (const QByteArray& key : keys){
			std::shared_ptr<CGqlFieldFragment> sourceObject = sourcePtr->m_fragmentFields.value(key);

			std::shared_ptr<CGqlFieldFragment> gqlFragment;

			std::shared_ptr<istd::IChangeable> clonedPtr(sourceObject->CloneMe().PopInterfacePtr());
			Q_ASSERT(clonedPtr != nullptr);

			gqlFragment = std::dynamic_pointer_cast<CGqlFieldFragment>(clonedPtr);
			if (gqlFragment == nullptr){
				Q_ASSERT(false);

				return false;
			}

			m_fragmentFields.insert(key, gqlFragment);
		}

		return true;
	}

	return false;
}


istd::IChangeableUniquePtr CGqlFieldObject::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CGqlFieldObject);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CGqlFieldObject::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_emptyFields.clear();
	m_parentPtr = nullptr;
	m_objectFields.clear();
	m_fragmentFields.clear();

	return true;
}


// protected methods

void CGqlFieldObject::RemoveField(const QByteArray &fieldId)
{
	m_emptyFields.removeAll(fieldId);
	m_objectFields.remove(fieldId);
	m_fragmentFields.remove(fieldId);
}


} // namespace imtgql



