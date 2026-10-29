// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtrest/CHttpServletCompBase.h>
#include <imtauth/IExternalLoginCallbackHandler.h>


namespace imtauthgql
{


/**
	Redirect URI of the external identity providers. The provider sends the browser here with
	'code' and 'state' as query parameters, or as form fields (response_mode=form_post, Apple).
	The browser shows the result page; the application gets the session by polling CompleteExternalLogin.
*/
class CExternalLoginCallbackServletComp: public imtrest::CHttpServletCompBase
{
public:
	typedef imtrest::CHttpServletCompBase BaseClass;

	I_BEGIN_COMPONENT(CExternalLoginCallbackServletComp);
		I_ASSIGN(m_callbackHandlerCompPtr, "CallbackHandler", "Handler finishing the login with the provider response", true, "ExternalLoginCallbackHandler");
	I_END_COMPONENT;

protected:
	// reimplemented (imtrest::CHttpServletCompBase)
	virtual imtrest::ConstResponsePtr OnGet(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const override;
	virtual imtrest::ConstResponsePtr OnPost(
				const QByteArray& commandId,
				const imtrest::IRequest::CommandParams& commandParams,
				const HeadersMap& headers,
				const imtrest::CHttpRequest& request) const override;

private:
	imtrest::ConstResponsePtr ProcessCallback(const imtrest::IRequest::CommandParams& params, const imtrest::CHttpRequest& request) const;
	imtrest::ConstResponsePtr CreatePageResponse(bool isSuccessful, const QString& message, const imtrest::CHttpRequest& request) const;

private:
	I_REF(imtauth::IExternalLoginCallbackHandler, m_callbackHandlerCompPtr);
};


} // namespace imtauthgql


