#include <imtbase/CObjectCollectionRemoveEvent.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>


namespace imtbase
{


// public methods

CObjectCollectionRemoveEvent::CObjectCollectionRemoveEvent()
{
}


CObjectCollectionRemoveEvent::CObjectCollectionRemoveEvent(const QByteArray& itemId)
	:BaseClass(itemId)
{
}


// reimplemented (istd::IChangeable)

bool CObjectCollectionRemoveEvent::CopyFrom(const IChangeable& object, CompatibilityMode mode)
{
	const CObjectCollectionRemoveEvent* sourcePtr = dynamic_cast<const CObjectCollectionRemoveEvent*>(&object);
	if (sourcePtr != nullptr){
		if (BaseClass::GetSupportedOperations() & SO_COPY){
			if (!BaseClass::CopyFrom(object, mode)){
				return false;
			}
		}

		return true;
	}

	return false;
}


bool CObjectCollectionRemoveEvent::IsEqual(const IChangeable& object) const
{
	const CObjectCollectionRemoveEvent* sourcePtr = dynamic_cast<const CObjectCollectionRemoveEvent*>(&object);
	if (sourcePtr != NULL){
		if (BaseClass::GetSupportedOperations() & SO_COMPARE){
			if (!BaseClass::IsEqual(object)){
				return false;
			}
		}

		return true;
	}

	return false;
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


