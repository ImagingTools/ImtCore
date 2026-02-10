// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/ILoginStatusProvider.h>
#include <imtclientgql/CSubscriberCompBase.h>


namespace imtclientgql
{


class CLoginStatusSubscriberComp: public CSubscriberCompBase, public imtauth::ILoginStatusProvider
{
public:
	typedef CSubscriberCompBase BaseClass;

	I_BEGIN_COMPONENT(CLoginStatusSubscriberComp);
		I_REGISTER_INTERFACE(imtauth::ILoginStatusProvider);
	I_END_COMPONENT;

	CLoginStatusSubscriberComp();

protected:
	// reimplemented (CSubscriberCompBase)
	virtual void OnResponseReceived(
				const QByteArray& subscriptionId,
				const QByteArray& subscriptionData) override;

	// reimplemented (imtauth::ILoginStatusProvider)
	virtual int GetLoginStatus(const QByteArray& clientId = QByteArray()) const override;

protected:
	imtauth::ILoginStatusProvider::LoginStatusFlags m_loginStatus;
};


} // namespace imtclientgql


