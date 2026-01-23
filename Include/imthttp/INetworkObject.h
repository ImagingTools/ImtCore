#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imthttp
{


class IProtocolEngine;


class INetworkObject: virtual public istd::IChangeable
{
public:
	virtual const IProtocolEngine& GetProtocolEngine() const = 0;
};


} // namespace imthttp


