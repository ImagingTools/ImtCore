#include <imtbase/COperationDescription.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>


namespace imtbase
{


// public methods

void COperationDescription::SetOperationTypeId(const QByteArray& operationTypeId)
{
	if (m_operationTypeId != operationTypeId){
		istd::CChangeNotifier changeNotifier(this);

		m_operationTypeId = operationTypeId;
	}
}


void COperationDescription::SetKey(const QByteArray& key)
{
	if (m_key != key){
		istd::CChangeNotifier changeNotifier(this);

		m_key = key;
	}
}


void COperationDescription::SetKeyName(const QString& keyName)
{
	if (m_keyName != keyName){
		istd::CChangeNotifier changeNotifier(this);

		m_keyName = keyName;
	}
}


void COperationDescription::SetOldValue(const QByteArray& oldValue)
{
	if (m_oldValue != oldValue){
		istd::CChangeNotifier changeNotifier(this);

		m_oldValue = oldValue;
	}
}


void COperationDescription::SetNewValue(const QByteArray& newValue)
{
	if (m_newValue != newValue){
		istd::CChangeNotifier changeNotifier(this);

		m_newValue = newValue;
	}
}


// reimplemented (imtbase::IOperationDescription)

QByteArray COperationDescription::GetOperationTypeId() const
{
	return m_operationTypeId;
}


QByteArray COperationDescription::GetKey() const
{
	return m_key;
}


QString COperationDescription::GetKeyName() const
{
	return m_keyName;
}


QByteArray COperationDescription::GetOldValue() const
{
	return m_oldValue;
}


QByteArray COperationDescription::GetNewValue() const
{
	return m_newValue;
}


// reimplemented (iser::ISerializable)

bool COperationDescription::Serialize(iser::IArchive& archive)
{
	bool retVal = true;

	iser::CArchiveTag operationTypeTag("OperationTypeId", "Operation type ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(operationTypeTag);
	retVal = retVal && archive.Process(m_operationTypeId);
	retVal = retVal && archive.EndTag(operationTypeTag);

	iser::CArchiveTag keyTag("Key", "Key of the field being modified", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(keyTag);
	retVal = retVal && archive.Process(m_key);
	retVal = retVal && archive.EndTag(keyTag);

	iser::CArchiveTag keyNameTag("KeyName", "Key name of the field being modified", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(keyNameTag);
	retVal = retVal && archive.Process(m_keyName);
	retVal = retVal && archive.EndTag(keyNameTag);

	iser::CArchiveTag oldValueTag("OldValue", "Old value", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(oldValueTag);
	retVal = retVal && archive.Process(m_oldValue);
	retVal = retVal && archive.EndTag(oldValueTag);

	iser::CArchiveTag newValueTag("NewValue", "New value", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(newValueTag);
	retVal = retVal && archive.Process(m_newValue);
	retVal = retVal && archive.EndTag(newValueTag);

	return retVal;
}


// reimplemented (istd::IChangeable)
int COperationDescription::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool COperationDescription::CopyFrom(const IChangeable& object, CompatibilityMode mode)
{
	istd::CChangeGroup changeGroup(this);

	const COperationDescription* sourcePtr = dynamic_cast<const COperationDescription*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_operationTypeId = sourcePtr->m_operationTypeId;
		m_key = sourcePtr->m_key;
		m_keyName = sourcePtr->m_keyName;
		m_oldValue = sourcePtr->m_oldValue;
		m_newValue = sourcePtr->m_newValue;

		return true;
	}

	return false;
}


istd::IChangeable* COperationDescription::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<COperationDescription> clonePtr(new COperationDescription);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool COperationDescription::ResetData(CompatibilityMode mode)
{
	m_operationTypeId.clear();
	m_key.clear();
	m_keyName.clear();
	m_oldValue.clear();
	m_newValue.clear();

	return true;
}


} // namespace imtbase


