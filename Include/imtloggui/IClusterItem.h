#pragma once


// ImtCore includes
#include <imtloggui/IEventItem.h>


namespace imtloggui
{


/**
	Interface for event item
*/
class IClusterItem: virtual public IItemBase
{
public:
	virtual void SetParams(const QSize& size) = 0;
	virtual void Attach(IEventItem* itemPtr) = 0;
	virtual void DetachAll() = 0;
};


} // namespace imtloggui


