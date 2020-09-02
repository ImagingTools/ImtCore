#pragma once


// ACF includes
#include <ilog/IMessageConsumer.h>

// ImtCore includes
#include <imtloggui/IItemBase.h>


namespace imtloggui
{


/**
	Interface for event item
*/
class IEventItem: virtual public IItemBase
{
public:
	/**
		Get pointer to source message
	*/
	virtual const istd::IInformationProvider* GetInformationProvider() const = 0;
};


} // namespace imtloggui


