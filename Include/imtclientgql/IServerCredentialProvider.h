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


} // namespace imtclientgql
