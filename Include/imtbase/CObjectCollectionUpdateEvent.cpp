#include <imtbase/CObjectCollectionUpdateEvent.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>


namespace imtbase
{


// public methods

CObjectCollectionUpdateEvent::CObjectCollectionUpdateEvent()
{
}


CObjectCollectionUpdateEvent::CObjectCollectionUpdateEvent(const QByteArray& itemId, UpdateType updateType, const QVariant oldValue, const QVariant newValue)
	:BaseClass(itemId),
	m_updateType(updateType),
	m_oldValue(oldValue),
	m_newValue(newValue)
{
}


CObjectCollectionUpdateEvent::UpdateType CObjectCollectionUpdateEvent::GetUpdateType() const
{
	return m_updateType;
}


QVariant CObjectCollectionUpdateEvent::GetOldValue() const
{
	return m_oldValue;
}


QVariant CObjectCollectionUpdateEvent::GetNewValue() const
{
	return m_newValue;
}


// reimplemented (IObjectCollectionEvent)

imtbase::IObjectCollectionEvent::EventType CObjectCollectionUpdateEvent::GetEventType() const
{
	return ET_UPDATE;
}


// reimplemented (istd::IChangeable)

int CObjectCollectionUpdateEvent::GetSupportedOperations() const
{
	return BaseClass::GetSupportedOperations() | SO_CLONE;
}


bool CObjectCollectionUpdateEvent::CopyFrom(const IChangeable& object, CompatibilityMode mode)
{
	const CObjectCollectionUpdateEvent* sourcePtr = dynamic_cast<const CObjectCollectionUpdateEvent*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeGroup changeGroup(this);

		if (!BaseClass::CopyFrom(object, mode)){
			return false;
		}

		if (m_updateType != sourcePtr->m_updateType){
			istd::CChangeNotifier notifier(this);

			m_updateType = sourcePtr->m_updateType;
		}

		if (m_oldValue != sourcePtr->m_oldValue) {
			istd::CChangeNotifier notifier(this);

			m_oldValue = sourcePtr->m_oldValue;
		}

		if (m_newValue != sourcePtr->m_newValue) {
			istd::CChangeNotifier notifier(this);

			m_newValue = sourcePtr->m_newValue;
		}

		return true;
	}

	return false;
}


bool CObjectCollectionUpdateEvent::IsEqual(const IChangeable& object) const
{
	const CObjectCollectionUpdateEvent* sourcePtr = dynamic_cast<const CObjectCollectionUpdateEvent*>(&object);
	if (sourcePtr != NULL){
		if (!BaseClass::IsEqual(object)){
			return false;
		}

		if (m_updateType != sourcePtr->m_updateType){
			return false;
		}

		return true;
	}

	return false;
}


istd::IChangeable* CObjectCollectionUpdateEvent::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CObjectCollectionUpdateEvent> clonePtr(new CObjectCollectionUpdateEvent);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


} // namespace imtbase


