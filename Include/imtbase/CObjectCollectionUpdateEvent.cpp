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


CObjectCollectionUpdateEvent::CObjectCollectionUpdateEvent(const QByteArray& itemId, UpdateType updateType)
	:BaseClass(itemId),
	m_updateType(updateType)
{
}


CObjectCollectionUpdateEvent::UpdateType CObjectCollectionUpdateEvent::GetUpdateType() const
{
	return m_updateType;
}


// reimplemented (istd::IChangeable)

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

		return true;
	}

	return false;
}


bool CObjectCollectionUpdateEvent::IsEqual(const IChangeable& object) const
{
	const CObjectCollectionUpdateEvent* sourcePtr = dynamic_cast<const CObjectCollectionUpdateEvent*>(&object);
	if (sourcePtr != NULL){
		if (BaseClass::GetSupportedOperations() & SO_COMPARE){
			if (!BaseClass::IsEqual(object)){
				return false;
			}
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


