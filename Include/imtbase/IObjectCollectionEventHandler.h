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
	virtual void OnObjectCollectionEvent(
				const imtbase::IObjectCollection* objectCollectionPtr,
				const imtbase::IObjectCollectionEvent* eventPtr) = 0;
};


} // namespace imtbase


