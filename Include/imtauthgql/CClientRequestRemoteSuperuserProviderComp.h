// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ibase/IApplicationInfo.h>

// ImtCore includes
#include <imtauth/ISuperuserProvider.h>
#include <imtclientgql/TClientRequestManagerCompWrap.h>


namespace imtauthgql
{


class CClientRequestRemoteSuperuserProviderComp:
			public imtclientgql::CClientRequestManagerCompBase,
			virtual public imtauth::ISuperuserProvider
{
public:
	typedef imtclientgql::CClientRequestManagerCompBase BaseClass;

	I_BEGIN_COMPONENT(CClientRequestRemoteSuperuserProviderComp)
		I_REGISTER_INTERFACE(imtauth::ISuperuserProvider);
		I_ASSIGN(m_superuserIdAttrPtr, "SuperuserId", "ID of the superuser", false, "su");
	I_END_COMPONENT;

	// reimplemented (imtauth::ISuperuserProvider)
	virtual ExistsStatus SuperuserExists(QString& errorMessage) const override;
	virtual QByteArray GetSuperuserId() const override;

protected:
	I_ATTR(QByteArray, m_superuserIdAttrPtr);
};


} // namespace imtauthgql


