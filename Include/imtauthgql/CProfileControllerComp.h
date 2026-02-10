// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtlic/IProductInfo.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Profile.h>


namespace imtauthgql
{


class CProfileControllerComp: public sdl::imtauth::Profile::CGraphQlHandlerCompBase
{
public:
	typedef sdl::imtauth::Profile::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CProfileControllerComp);
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "User collection", false, "UserCollection");
		I_ASSIGN(m_roleCollectionCompPtr, "RoleCollection", "Role collection", false, "RoleCollection");
		I_ASSIGN(m_groupCollectionCompPtr, "GroupCollection", "Group collection", false, "GroupCollection");
		I_ASSIGN(m_productInfoCompPtr, "ProductInfo", "Product feature container", false, "ProductInfo");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtauth::Profile::CGraphQlHandlerCompBase)
	virtual sdl::imtauth::Profile::CProfileData OnGetProfile(
				const sdl::imtauth::Profile::CGetProfileGqlRequest& getProfileRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Profile::CSetProfileResponse OnSetProfile(
				const sdl::imtauth::Profile::CSetProfileGqlRequest& setProfileRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
protected:
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_roleCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_groupCollectionCompPtr);
	I_REF(imtlic::IProductInfo, m_productInfoCompPtr);
};


} // namespace imtauthgql


