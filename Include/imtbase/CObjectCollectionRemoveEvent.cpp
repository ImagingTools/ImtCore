#include <imtbase/CObjectCollectionRemoveEvent.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>


namespace imtbase
{


// public methods

CObjectCollectionRemoveEvent::CObjectCollectionRemoveEvent(const QByteArray& itemId)
	:BaseClass(itemId)
{
}


// reimplemented (IObjectCollectionEvent)

imtbase::IObjectCollectionEvent::EventType CObjectCollectionRemoveEvent::GetEventType() const
{
	return ET_REMOVE;
}


// reimplemented (istd::IChangeable)

int CObjectCollectionRemoveEvent::GetSupportedOperations() const
{
	return BaseClass::GetSupportedOperations() | SO_CLONE;
}


istd::IChangeable* CObjectCollectionRemoveEvent::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CObjectCollectionRemoveEvent> clonePtr(new CObjectCollectionRemoveEvent);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


} // namespace imtbase


