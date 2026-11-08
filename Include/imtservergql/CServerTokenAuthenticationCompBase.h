// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtauth/IPersonalAccessTokenManager.h>
#include <imtservergql/IProductServerRequestPolicy.h>


namespace imtservergql
{


/**
	What the authentication knows about the tokens of servers. A base of its own: one component
	function can hold only a limited number of static objects, and the manager reached it.
*/
class CServerTokenAuthenticationCompBase: public ilog::CLoggerComponentBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CServerTokenAuthenticationCompBase);
		I_ASSIGN(m_serverProductIdAttrPtr, "ServerProductId", "ID of this server in the tokens of other servers. Set, it refuses a request of another product without a token of its server", false, "");
		I_ASSIGN(m_requestPolicyCompPtr, "ProductServerRequestPolicy", "What the servers of products may send; without it they may send nothing", false, "ProductServerRequestPolicy");
		I_ASSIGN(m_serverTokenManagerCompPtr, "ServerTokenManager", "Where the tokens this server issued for other servers are kept; without it they are checked by the personal access token manager", false, "ServerTokenManager");
	I_END_COMPONENT;

protected:
	I_ATTR(QByteArray, m_serverProductIdAttrPtr);
	I_REF(IProductServerRequestPolicy, m_requestPolicyCompPtr);
	I_REF(imtauth::IPersonalAccessTokenManager, m_serverTokenManagerCompPtr);
};


} // namespace imtservergql


