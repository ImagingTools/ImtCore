// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>

// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtgql/IGqlContext.h>


namespace imtclientgql
{


/**
	Session of this application with a remote server: its own account there.
	A request sent with the context of the session goes on behalf of that account,
	not on behalf of the user whose request is being handled here.
*/
class IRemoteSession: virtual public istd::IPolymorphic
{
public:
	/**
		\brief Creates the context of a request to the remote server: the token of the session and the product of the server.
		Signs in first when the session holds no token.
		\param error [OUTPUT] - why there is no context, e.g. the sign-in was refused
		\return the context, an invalid pointer on failure
	*/
	virtual imtgql::IGqlContextSharedPtr CreateRequestContext(QString& error) = 0;

	/**
		\brief Tells the session that the remote server refused the token of one of its contexts.
		The next context comes with a new token, unless the token has been replaced already.
		\param token [INPUT] - the refused token
	*/
	virtual void OnTokenRejected(const QByteArray& token) = 0;
};


} // namespace imtclientgql
