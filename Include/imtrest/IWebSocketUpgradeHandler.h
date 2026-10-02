// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


class QTcpSocket;


namespace imtrest
{


/**
	Interface for taking over a TCP connection that requested an upgrade to the WebSocket protocol.
	It allows an HTTP server to serve WebSocket connections on its own port.
*/
class IWebSocketUpgradeHandler: virtual public istd::IPolymorphic
{
public:
	/**
		Take over the socket and complete the WebSocket handshake.
		\param socketPtr	Connected socket without parent. The unread HTTP upgrade request must still be in its read buffer.
							The method must be called from the thread owning the socket.
		\return	\c true if the ownership of the socket was taken, otherwise the caller stays responsible for the socket.
	*/
	virtual bool HandleWebSocketUpgrade(QTcpSocket* socketPtr) = 0;
};


} // namespace imtrest


