#pragma once


// Acf includes
#include <istd/IPolymorphic.h>


namespace imtloggui
{


class ILayerFinder: virtual public istd::IPolymorphic
{
public:
	virtual QByteArray GetLayerIdForTimespan(uint64_t timespan) const = 0;
};


} // namespace imtloggui


