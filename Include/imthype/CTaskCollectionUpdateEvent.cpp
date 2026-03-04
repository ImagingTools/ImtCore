// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imthype/CTaskCollectionUpdateEvent.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>


namespace imthype
{


// public methods

CTaskCollectionUpdateEvent::CTaskCollectionUpdateEvent() : m_updateType(UT_USER_TASK_ID)
{
}


CTaskCollectionUpdateEvent::CTaskCollectionUpdateEvent(
			const QByteArray& itemId,
			UpdateType updateType, const QVariant& oldValue, const QVariant& newValue)
	:m_itemId(itemId),
	m_updateType(updateType),
	m_oldValue(oldValue),
	m_newValue(newValue)
{
}


CTaskCollectionUpdateEvent::UpdateType CTaskCollectionUpdateEvent::GetUpdateType() const
{
	return m_updateType;
}


QVariant CTaskCollectionUpdateEvent::GetOldValue() const
{
	return m_oldValue;
}


QVariant CTaskCollectionUpdateEvent::GetNewValue() const
{
	return m_newValue;
}


// reimplemented (istd::IChangeable)

int CTaskCollectionUpdateEvent::GetSupportedOperations() const
{
	return SO_COMPARE | SO_COPY | SO_CLONE;
}


bool CTaskCollectionUpdateEvent::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	auto sourcePtr = dynamic_cast<const CTaskCollectionUpdateEvent*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeGroup changeGroup(this);

		if (m_updateType != sourcePtr->m_updateType){
			istd::CChangeNotifier notifier(this);

			m_updateType = sourcePtr->m_updateType;
		}

		if (m_oldValue != sourcePtr->m_oldValue){
			istd::CChangeNotifier notifier(this);

			m_oldValue = sourcePtr->m_oldValue;
		}

		if (m_newValue != sourcePtr->m_newValue){
			istd::CChangeNotifier notifier(this);

			m_newValue = sourcePtr->m_newValue;
		}

		return true;
	}

	return false;
}


bool CTaskCollectionUpdateEvent::IsEqual(const IChangeable& object) const
{
	auto sourcePtr = dynamic_cast<const CTaskCollectionUpdateEvent*>(&object);
	if (sourcePtr != nullptr){
		return m_updateType == sourcePtr->m_updateType;
	}

	return false;
}


istd::IChangeableUniquePtr CTaskCollectionUpdateEvent::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CTaskCollectionUpdateEvent);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


} // namespace imthype


