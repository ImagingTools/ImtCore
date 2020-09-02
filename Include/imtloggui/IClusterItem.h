#pragma once


// ImtCore includes
#include <imtloggui/IEventItem.h>


namespace imtloggui
{


/**
	Interface for cluster item
*/
class IClusterItem:	virtual public IItemBase
{
public:
	virtual void Attach(IEventItem* itemPtr) = 0;
	virtual void DetachAll() = 0;
};


} // namespace imtloggui


