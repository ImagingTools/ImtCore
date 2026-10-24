// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


class QTcpSocket;


namespace imtrest
{


/**
	Interface for taking over a WebSocket upgrade request, that was received on the HTTP port.
	It allows to serve HTTP(S) and WebSocket(S) on the same port: the HTTP server detects the upgrade
	request and hands the raw (already TLS-decrypted, if secure) socket over to the WebSocket server.
	The upgrade request itself was not read from the socket, so the WebSocket server performs the complete handshake.
*/
class IWebSocketUpgradeHandler: virtual public istd::IPolymorphic
{
public:
	/**
		Take over the connected socket with a pending WebSocket upgrade request.
		The method is called in the thread of the implementing object, the socket already lives in this thread and has no parent.
		\param socketPtr	Socket instance. The ownership is transferred to the handler, also if the method fails.
		\return \c true if the handshake was started, otherwise \c false.
	*/
	virtual bool HandleWebSocketHandshake(QTcpSocket* socketPtr) = 0;
};


} // namespace imtrest


