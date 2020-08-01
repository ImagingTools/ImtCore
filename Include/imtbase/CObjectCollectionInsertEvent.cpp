#include <imtbase/CObjectCollectionInsertEvent.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>


namespace imtbase
{


// public methods

CObjectCollectionInsertEvent::CObjectCollectionInsertEvent()
{
}

CObjectCollectionInsertEvent::CObjectCollectionInsertEvent(const QByteArray& itemId)
	:BaseClass(itemId)
{
}


// reimplemented (istd::IChangeable)

bool CObjectCollectionInsertEvent::CopyFrom(const IChangeable& object, CompatibilityMode mode)
{
	const CObjectCollectionInsertEvent* sourcePtr = dynamic_cast<const CObjectCollectionInsertEvent*>(&object);
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


bool CObjectCollectionInsertEvent::IsEqual(const IChangeable& object) const
{
	const CObjectCollectionInsertEvent* sourcePtr = dynamic_cast<const CObjectCollectionInsertEvent*>(&object);
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


istd::IChangeable* CObjectCollectionInsertEvent::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CObjectCollectionInsertEvent> clonePtr(new CObjectCollectionInsertEvent);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


} // namespace imtbase


