#pragma once


// ImtCore includes
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <imtbase/IObjectCollection.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Profile.h>


namespace imtauthgql
{


class CProfileControllerComp: public sdl::imtauth::Profile::V1_0::CGraphQlHandlerCompBase
{
public:
	typedef sdl::imtauth::Profile::V1_0::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CProfileControllerComp);
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "User collection", false, "UserCollection");
		I_ASSIGN(m_roleCollectionCompPtr, "RoleCollection", "Role collection", false, "RoleCollection");
		I_ASSIGN(m_groupCollectionCompPtr, "GroupCollection", "Group collection", false, "GroupCollection");
		I_ASSIGN(m_featureCollectionCompPtr, "FeatureCollection", "Feature collection", false, "FeatureCollection");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtauth::Profile::V1_0::CGraphQlHandlerCompBase)
	virtual sdl::imtauth::Profile::V1_0::CProfileData OnGetProfile(
				const sdl::imtauth::Profile::V1_0::CGetProfileGqlRequest& getProfileRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Profile::V1_0::CSetProfileResponse OnSetProfile(
				const sdl::imtauth::Profile::V1_0::CSetProfileGqlRequest& setProfileRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
protected:
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_roleCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_groupCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_featureCollectionCompPtr);
};


} // namespace imtauthgql


