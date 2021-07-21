#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtWebSockets/QWebSocket>

// ACF includes
#include <istd/IChangeable.h>


namespace imtrest
{


class IProtocolEngine;


class INetworkObject: virtual public istd::IChangeable
{
public:
	virtual const IProtocolEngine& GetProtocolEngine() const = 0;
	virtual QObject& GetSocketObject() const = 0;
	virtual QWebSocket& GetSocketWs() const
	{
		QWebSocket *s =nullptr;
		return *s;
	}
};


} // namespace imtrest


