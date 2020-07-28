#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


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
	virtual void OnEvent(const IObjectCollection* objectCollection, const IObjectCollectionEvent* event) = 0;
};


} // namespace imtbase


