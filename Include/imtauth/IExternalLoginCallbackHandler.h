// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtauth
{


/**
	Receiver of the redirect from the external identity provider back to the server.

	\ingroup Authorization
*/
class IExternalLoginCallbackHandler: virtual public istd::IPolymorphic
{
public:
	/**
		Finish the login identified by the state parameter.
		\param providerError	error reported by the provider instead of a code, empty on success.
		\param message			[out] text shown to the user in the browser.
		\return true if the login succeeded.
	*/
	virtual bool HandleCallback(
				const QByteArray& state,
				const QByteArray& code,
				const QString& providerError,
				QString& message) const = 0;
};


} // namespace imtauth


