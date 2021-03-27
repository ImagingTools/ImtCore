#include <imtbase/CObjectCollectionInsertEvent.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>


namespace imtbase
{


// public methods

CObjectCollectionInsertEvent::CObjectCollectionInsertEvent(const QByteArray& itemId)
	:BaseClass(itemId)
{
}


// reimplemented (IObjectCollectionEvent)

int CObjectCollectionInsertEvent::GetEventType() const
{
	return ET_INSERT;
}


// reimplemented (istd::IChangeable)

int CObjectCollectionInsertEvent::GetSupportedOperations() const
{
	return BaseClass::GetSupportedOperations() | SO_CLONE;
}


istd::IChangeable* CObjectCollectionInsertEvent::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CObjectCollectionInsertEvent> clonePtr(new CObjectCollectionInsertEvent);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


} // namespace imtbase


