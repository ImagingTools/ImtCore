// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtauth/IPersonalAccessTokenManager.h>


namespace imtservergql
{


/**
	How this server receives the servers of other products: its name in their tokens and where it keeps the tokens it issued for them.
*/
class IServerTokenReceiver: virtual public istd::IPolymorphic
{
public:
	/**
		Get the product of this server: the tokens it issues are for it.
	*/
	virtual QByteArray GetServerProductId() const = 0;

	/**
		Get the manager of the tokens this server issued for other servers.
	*/
	virtual imtauth::IPersonalAccessTokenManager* GetTokenManager() const = 0;
};


} // namespace imtservergql
