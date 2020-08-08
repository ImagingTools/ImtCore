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


// reimplemented (istd::IChangeable)

istd::IChangeable* CObjectCollectionInsertEvent::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CObjectCollectionInsertEvent> clonePtr(new CObjectCollectionInsertEvent);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


} // namespace imtbase


