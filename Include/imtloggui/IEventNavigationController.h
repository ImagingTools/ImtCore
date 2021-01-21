#pragma once


// Acf includes
#include <istd/IPolymorphic.h>


namespace imtloggui
{


class IEventNavigationController: virtual public istd::IPolymorphic
{
public:
	virtual void JumpToFirstEvent() const = 0;
	virtual void JumpToPreviousEvent() const = 0;
	virtual void JumpToNextEvent() const = 0;
	virtual void JumpToLastEvent() const = 0;
};


} // namespace imtloggui


