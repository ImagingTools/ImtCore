// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtclientgql/IServerCredentialProvider.h>


namespace imtclientgql
{


/**
	Reads the token of this server from an environment variable of the service, never from a setting or a log.
*/
class CServerCredentialProviderComp:
			public ilog::CLoggerComponentBase,
			virtual public IServerCredentialProvider
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CServerCredentialProviderComp);
		I_REGISTER_INTERFACE(IServerCredentialProvider);
		I_ASSIGN(m_serverProductIdAttrPtr, "ServerProductId", "Product of the server the token is for; it names the environment variable holding the token", true, "");
	I_END_COMPONENT;

	// reimplemented (imtclientgql::IServerCredentialProvider)
	virtual QByteArray GetServerToken() const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_ATTR(QByteArray, m_serverProductIdAttrPtr);

	QByteArray m_token;
};


} // namespace imtclientgql
