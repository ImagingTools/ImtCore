#pragma once


// Qt includes
#include <QtNetwork/QAbstractSocket>

// ACF includes
#include <istd/IChangeable.h>


namespace imtrest
{


class IProtocolEngine;


class INetworkObject: virtual public istd::IChangeable
{
public:
	virtual const IProtocolEngine& GetProtocolEngine() const = 0;
	virtual const QAbstractSocket& GetSocket() const = 0;
};


} // namespace imtrest


