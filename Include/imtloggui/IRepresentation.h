#pragma once


// ACF includes
#include <istd/IChangeable.h>
#include <imod/IObserver.h>


namespace imtloggui
{


class IRepresentation: virtual public istd::IChangeable
{
	virtual imod::IObserver* GetTimeRangeObserver() = 0;
};


} // namespace imtloggui


