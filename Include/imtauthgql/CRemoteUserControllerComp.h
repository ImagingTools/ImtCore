// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/ISuperuserProvider.h>
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Users>


namespace imtauthgql
{


class CRemoteUserControllerComp:
			virtual public imtauth::ISuperuserProvider,
			public imtclientgql::TClientRequestManagerCompWrap<
						sdl::V1_0::imtauth::CUsersGqlHandlerCompBase>
{
public:
	typedef imtclientgql::TClientRequestManagerCompWrap<sdl::V1_0::imtauth::CUsersGqlHandlerCompBase> BaseClass;

	I_BEGIN_COMPONENT(CRemoteUserControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (imtauth::ISuperuserProvider)
	virtual ExistsStatus SuperuserExists(QString& errorMessage) const override;
	virtual QByteArray GetSuperuserId() const override;

	// reimplemented (sdl::V1_0::imtauth::CUsersGqlHandlerCompBase)
	virtual sdl::V1_0::imtauth::CChangePasswordPayload OnChangePassword(
				const sdl::V1_0::imtauth::CChangePasswordGqlRequest& changePasswordRequest,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CRegisterUserPayload OnRegisterUser(
				const sdl::V1_0::imtauth::CRegisterUserGqlRequest& registerUserRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CCheckEmailPayload OnCheckEmail(
				const sdl::V1_0::imtauth::CCheckEmailGqlRequest& checkEmailRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CSendEmailCodePayload OnSendEmailCode(
				const sdl::V1_0::imtauth::CSendEmailCodeGqlRequest& sendEmailCodeRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CCheckEmailCodePayload OnCheckEmailCode(
				const sdl::V1_0::imtauth::CCheckEmailCodeGqlRequest& checkEmailCodeRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CCheckSuperuserPayload OnCheckSuperuserExists(
				const sdl::V1_0::imtauth::CCheckSuperuserExistsGqlRequest& checkSuperuserExistsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CCreateSuperuserPayload OnCreateSuperuser(
				const sdl::V1_0::imtauth::CCreateSuperuserGqlRequest& createSuperuserRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtauth::CUserObjectId OnGetUserObjectId(
				const sdl::V1_0::imtauth::CGetUserObjectIdGqlRequest& getUserObjectIdRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
};


} // namespace imtauthgql


