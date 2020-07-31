#pragma once


// ACF includes
#include <istd/IPolymorphic.h>
#include <istd/TSmartPtr.h>


namespace imtbase
{

class IObjectCollection;
class IObjectCollectionEvent;

/**
	Interface for object collection event handler
*/
class IObjectCollectionEventHandler: virtual public istd::IPolymorphic
{
public:
	typedef const imtbase::IObjectCollection* ObjectCollectionPtr;
	typedef istd::TSmartPtr<IObjectCollectionEvent> ObjectCollectionEventPtr;

	virtual void OnObjectCollectionEvent(
				ObjectCollectionPtr objectCollectionPtr,
				ObjectCollectionEventPtr eventPtr) = 0;
};


} // namespace imtbase


