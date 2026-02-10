// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtservergql/CObjectCollectionControllerCompBase.h>
#include <imtcrypt/IHashGenerator.h>
#include <imtauth/IUserGroupInfoProvider.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Users.h>


namespace imtauthgql
{


class CUserCollectionControllerComp: public sdl::imtauth::Users::CUserCollectionControllerCompBase
{
public:
	typedef sdl::imtauth::Users::CUserCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CUserCollectionControllerComp);
		I_ASSIGN(m_roleInfoProviderCompPtr, "RoleInfoProvider", "Role info provider", true, "RoleInfoProvider");
		I_ASSIGN(m_userGroupInfoProviderCompPtr, "UserGroupInfoProvider", "User group info provider", true, "UserGroupInfoProvider");
		I_ASSIGN(m_userInfoFactCompPtr, "UserFactory", "Factory used for creation of the new user instance", true, "UserFactory");
		I_ASSIGN(m_hashCalculatorCompPtr, "HashCalculator", "Hash calculator", true, "HashCalculator");
	I_END_COMPONENT;

protected:
	virtual bool FillObjectFromRepresentation(
				const sdl::imtauth::Users::CUserData::V1_0& representation,
				istd::IChangeable& object,
				QByteArray& newObjectId,
				QString& errorMessage) const;

	// reimplemented (sdl::imtbase::ImtCollection::CGraphQlHandlerCompBase)
	virtual sdl::imtbase::ImtCollection::CVisualStatus OnGetObjectVisualStatus(
				const sdl::imtbase::ImtCollection::CGetObjectVisualStatusGqlRequest& getObjectVisualStatusRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::ImtCollection::CGetElementMetaInfoPayload OnGetElementMetaInfo(
				const sdl::imtbase::ImtCollection::CGetElementMetaInfoGqlRequest& getElementMetaInfoRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	// reimplemented (sdl::imtauth::Users::CUserCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::imtauth::Users::CUsersListGqlRequest& usersListRequest,
				sdl::imtauth::Users::CUserItemData::V1_0& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeableUniquePtr CreateObjectFromRepresentation(
				const sdl::imtauth::Users::CUserData::V1_0& userDataRepresentation,
				QByteArray& newObjectId,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::imtauth::Users::CUserItemGqlRequest& userItemRequest,
				sdl::imtauth::Users::CUserData::V1_0& representationPayload,
				QString& errorMessage) const override;
	virtual bool UpdateObjectFromRepresentationRequest(
				const ::imtgql::CGqlRequest& rawGqlRequest,
				const sdl::imtauth::Users::CUserUpdateGqlRequest& userUpdateRequest,
				istd::IChangeable& object,
				QString& errorMessage) const override;

	// reimplemented (imtservergql::CPermissibleGqlRequestHandlerComp)
	virtual bool CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_REF(imtauth::IRoleInfoProvider, m_roleInfoProviderCompPtr);
	I_REF(imtauth::IUserGroupInfoProvider, m_userGroupInfoProviderCompPtr);
	I_FACT(imtauth::IUserInfo, m_userInfoFactCompPtr);
	I_REF(imtcrypt::IHashGenerator, m_hashCalculatorCompPtr);
};


} // namespace imtauthgql


