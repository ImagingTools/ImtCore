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


// reimplemented (istd::IChangeable)

istd::IChangeable* CObjectCollectionRemoveEvent::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CObjectCollectionRemoveEvent> clonePtr(new CObjectCollectionRemoveEvent);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


} // namespace imtbase


