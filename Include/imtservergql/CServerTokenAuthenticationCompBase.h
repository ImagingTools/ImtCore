// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtauth/IPersonalAccessTokenManager.h>
#include <imtservergql/IServerTokenReceiver.h>


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
		I_ASSIGN(m_serverTokenReceiverCompPtr, "ServerTokenReceiver", "How this server receives the servers of other products. Set, a request of another product needs a token of its server; without it this server accepts no server tokens", false, "ServerTokenReceiver");
	I_END_COMPONENT;

protected:
	I_REF(IServerTokenReceiver, m_serverTokenReceiverCompPtr);
};


} // namespace imtservergql


