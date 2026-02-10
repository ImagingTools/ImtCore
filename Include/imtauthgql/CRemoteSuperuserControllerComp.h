// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ibase/IApplicationInfo.h>

// ImtCore includes
#include <imtauth/ISuperuserController.h>
#include <imtclientgql/TClientRequestManagerCompWrap.h>


namespace imtauthgql
{


class CRemoteSuperuserControllerComp:
			public imtclientgql::CClientRequestManagerCompBase,
			virtual public imtauth::ISuperuserController
{
public:
	typedef imtclientgql::CClientRequestManagerCompBase BaseClass;

	I_BEGIN_COMPONENT(CRemoteSuperuserControllerComp)
		I_REGISTER_INTERFACE(imtauth::ISuperuserController);
		I_ASSIGN(m_applicationInfoCompPtr, "ApplicationInfo", "Application Info", true, "");
		I_ASSIGN(m_superuserNameAttrPtr, "SuperuserName", "Superuser name", false, "superuser");
		I_ASSIGN(m_superuserMailAttrPtr, "SuperuserMail", "Superuser mail", false, "superuser");
	I_END_COMPONENT;

	// reimplemented (ISuperuserController)
	virtual bool SetSuperuserPassword(const QByteArray& password) const override;

private:
	I_REF(ibase::IApplicationInfo, m_applicationInfoCompPtr);
	I_ATTR(QString, m_superuserNameAttrPtr);
	I_ATTR(QString, m_superuserMailAttrPtr);
};


} // namespace imtauthgql


