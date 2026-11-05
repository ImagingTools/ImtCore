// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtauth
{


/**
	The resolved personal access tokens kept by the authentication.
*/
class IPersonalAccessTokenCache: virtual public istd::IPolymorphic
{
public:
	/**
		Forget a token, so its next request is validated again.
	*/
	virtual void ForgetToken(const QByteArray& tokenId) = 0;
};


} // namespace imtauth
