// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtNetwork/QSslConfiguration>
#include <QtNetwork/QSslSocket>


namespace imtrest
{


/**
	Tools of the TLS of the server side of a connection.
*/
class CServerSslTools
{
public:
	/**
		Apply the configuration of the server to the socket of an accepted connection before the handshake
		(\c startServerEncryption()).
		The certificate chain (the first certificate is the certificate of the server, the others are the intermediate ones that
		a client needs to verify it), the private key, the protocol and the CA certificates are taken from the configuration.
		Everything else (the ciphers, the elliptic curves and so on) stays as the socket has it by default: the configuration of the
		server is usually made from nothing, and its empty fields must not replace the defaults, otherwise the handshake has no ciphers.
		A field that the configuration does not have (no certificate, no key, no CA certificates) is not changed.
		\param	socket			Socket of the connection that is not encrypted yet.
		\param	configuration	Configuration of the server, see \c imtcom::ISslConfigurationManager.
	*/
	static void ApplyConfiguration(QSslSocket& socket, const QSslConfiguration& configuration);
};


} // namespace imtrest


