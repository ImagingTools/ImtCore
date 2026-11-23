// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imod/IModel.h>
#include <imtauth/ISessionAuthorizationProvider.h>
#include <imtservergql/IServerTokenReceiver.h>


namespace imtservergql
{


/**
	Part of the references of the authentication manager: one component function can hold only a limited number of static
	objects (MSVC C2603), and the manager reached it.
*/
class CAuthenticationManagerCompBase: public ilog::CLoggerComponentBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CAuthenticationManagerCompBase);
		I_ASSIGN(m_serverTokenReceiverCompPtr, "ServerTokenReceiver", "How this server receives the servers of other products. Set, a request of another product needs a token of its server; without it this server accepts no server tokens", false, "ServerTokenReceiver");
		I_ASSIGN(m_roleCollectionCompPtr, "RoleCollection", "Roles, read at their source for the check of a subscription", false, "RoleCollection");
		I_ASSIGN(m_userGroupCollectionCompPtr, "UserGroupCollection", "User groups, read at their source for the check of a subscription", false, "UserGroupCollection");
		I_ASSIGN(m_sessionAuthorizationProviderCompPtr, "SessionAuthorizationProvider", "Set on a product server whose users the authorization server computes: a session is authorized there, and the user of a personal token is read at its source with only the permissions it was handed out with", false, "SessionAuthorizationProvider");
		I_ASSIGN(m_sessionCollectionModelCompPtr, "SessionCollection", "Sessions of this server: a removed session ends its access tokens at once, whoever removed it", false, "SessionCollection");
		I_ASSIGN_MULTI_0(m_accessModelsCompPtr, "AccessModels", "Models whose changes may change the access of tokens: users, roles, groups, memberships, organizations. What was computed for the tokens is not used after a change", false);
	I_END_COMPONENT;

protected:
	I_REF(IServerTokenReceiver, m_serverTokenReceiverCompPtr);
	I_REF(imtbase::IObjectCollection, m_roleCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_userGroupCollectionCompPtr);
	I_REF(imtauth::ISessionAuthorizationProvider, m_sessionAuthorizationProviderCompPtr);
	I_MULTIREF(imod::IModel, m_accessModelsCompPtr);
	I_REF(imod::IModel, m_sessionCollectionModelCompPtr);
};


} // namespace imtservergql


