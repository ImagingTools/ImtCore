// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// Qt includes
#include <QtCore/QByteArray>


namespace imtclientgql
{


/**
	The token identifying this server at another server.
	Only requests the server itself creates may use it, never a request made for a client:
	an anonymous browser would get the rights of the server through such a request.
*/
class IServerCredentialProvider: virtual public istd::IPolymorphic
{
public:
	/**
		\return The token, empty when this server has none.
	*/
	virtual QByteArray GetServerToken() const = 0;
};


/**
	Name of the environment variable a server reads its token for the server of the given product from,
	e.g. IMT_PUMA_SERVER_CREDENTIAL; whoever starts the server passes the token in it.
*/
inline QByteArray GetServerCredentialVariableName(const QByteArray& serverProductId)
{
	return QByteArrayLiteral("IMT_") + serverProductId.toUpper() + QByteArrayLiteral("_SERVER_CREDENTIAL");
}


} // namespace imtclientgql
