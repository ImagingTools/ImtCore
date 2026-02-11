// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/ISuperuserProvider.h>
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Users.h>


namespace imtauthgql
{


class CRemoteUserControllerComp:
			virtual public imtauth::ISuperuserProvider,
			public imtclientgql::TClientRequestManagerCompWrap<
						sdl::imtauth::Users::CGraphQlHandlerCompBase>
{
public:
	typedef imtclientgql::TClientRequestManagerCompWrap<sdl::imtauth::Users::CGraphQlHandlerCompBase> BaseClass;

	I_BEGIN_COMPONENT(CRemoteUserControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (imtauth::ISuperuserProvider)
	virtual ExistsStatus SuperuserExists(QString& errorMessage) const override;
	virtual QByteArray GetSuperuserId() const override;

	// reimplemented (sdl::imtauth::Users::CGraphQlHandlerCompBase)
	virtual sdl::imtauth::Users::CChangePasswordPayload OnChangePassword(
				const sdl::imtauth::Users::CChangePasswordGqlRequest& changePasswordRequest,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Users::CRegisterUserPayload OnRegisterUser(
				const sdl::imtauth::Users::CRegisterUserGqlRequest& registerUserRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Users::CCheckEmailPayload OnCheckEmail(
				const sdl::imtauth::Users::CCheckEmailGqlRequest& checkEmailRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Users::CSendEmailCodePayload OnSendEmailCode(
				const sdl::imtauth::Users::CSendEmailCodeGqlRequest& sendEmailCodeRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Users::CCheckEmailCodePayload OnCheckEmailCode(
				const sdl::imtauth::Users::CCheckEmailCodeGqlRequest& checkEmailCodeRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Users::CCheckSuperuserPayload OnCheckSuperuserExists(
				const sdl::imtauth::Users::CCheckSuperuserExistsGqlRequest& checkSuperuserExistsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Users::CCreateSuperuserPayload OnCreateSuperuser(
				const sdl::imtauth::Users::CCreateSuperuserGqlRequest& createSuperuserRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtauth::Users::CUserObjectId OnGetUserObjectId(
				const sdl::imtauth::Users::CGetUserObjectIdGqlRequest& getUserObjectIdRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
};


} // namespace imtauthgql


