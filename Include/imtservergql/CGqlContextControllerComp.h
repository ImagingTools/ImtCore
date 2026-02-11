// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtgql/IGqlContextController.h>
#include <imtauth/IJwtSessionController.h>
#include <imtauth/IPersonalAccessTokenManager.h>


namespace imtservergql
{


class CGqlContextControllerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtgql::IGqlContextController
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGqlContextControllerComp);
		I_REGISTER_INTERFACE(imtgql::IGqlContextController);
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "User collection", false, "UserCollection");
		I_ASSIGN(m_userSettingsCollectionCompPtr, "UserSettingsCollection", "User settings collection", false, "UserSettingsCollection");
		I_ASSIGN(m_jwtSessionControllerCompPtr, "JwtSessionController", "JWT session controller", false, "JwtSessionController");
		I_ASSIGN(m_patManagerCompPtr, "PersonalAccessTokenManager", "Personal Access Token manager", false, "PersonalAccessTokenManager");
	I_END_COMPONENT;

	// reimplemented (imtgql::IGqlContextController)
	virtual imtgql::IGqlContext* GetRequestContext(
				const imtgql::CGqlRequest& gqlRequest,
				const QByteArray& token,
				const imtgql::IGqlContext::Headers& headers,
				QString& errorMessage) const override;

private:
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_userSettingsCollectionCompPtr);
	I_REF(imtauth::IJwtSessionController, m_jwtSessionControllerCompPtr);
	I_REF(imtauth::IPersonalAccessTokenManager, m_patManagerCompPtr);
};


} // namespace imtservergql


