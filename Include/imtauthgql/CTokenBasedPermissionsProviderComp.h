// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/ITokenBasedPermissionsProvider.h>
#include <imtclientgql/TClientRequestManagerCompWrap.h>


namespace imtauthgql
{


class CTokenBasedPermissionsProviderComp:
			public imtclientgql::CClientRequestManagerCompBase,
			virtual public imtauth::ITokenBasedPermissionsProvider
{
public:
	typedef imtclientgql::CClientRequestManagerCompBase BaseClass;

	I_BEGIN_COMPONENT(CTokenBasedPermissionsProviderComp);
		I_REGISTER_INTERFACE(imtauth::ITokenBasedPermissionsProvider);
	I_END_COMPONENT;

	// reimplemented (imtauth::ITokenBasedPermissionsProvider)
	virtual const QByteArrayList GetPermissions(const QByteArray& token) const override;
};


} // namespace imtauthgql


