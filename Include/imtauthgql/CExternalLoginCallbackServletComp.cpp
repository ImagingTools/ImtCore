// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CExternalLoginCallbackServletComp.h>


// Qt includes
#include <QtCore/QUrlQuery>

// ImtCore includes
#include <imtrest/IProtocolEngine.h>
#include <imtrest/IResponse.h>


namespace imtauthgql
{


// protected methods

// reimplemented (imtrest::CHttpServletCompBase)

imtrest::ConstResponsePtr CExternalLoginCallbackServletComp::OnGet(
			const QByteArray& /*commandId*/,
			const imtrest::IRequest::CommandParams& commandParams,
			const HeadersMap& /*headers*/,
			const imtrest::CHttpRequest& request) const
{
	return ProcessCallback(commandParams, request);
}


imtrest::ConstResponsePtr CExternalLoginCallbackServletComp::OnPost(
			const QByteArray& /*commandId*/,
			const imtrest::IRequest::CommandParams& /*commandParams*/,
			const HeadersMap& /*headers*/,
			const imtrest::CHttpRequest& request) const
{
	// application/x-www-form-urlencoded: '+' encodes a space.
	QByteArray body = request.GetBody();
	body.replace('+', ' ');

	imtrest::IRequest::CommandParams params;

	const QUrlQuery query(QString::fromUtf8(body));
	const QList<QPair<QString, QString>> items = query.queryItems(QUrl::FullyDecoded);
	for (const QPair<QString, QString>& item : items){
		params.insert(item.first.toUtf8(), item.second.toUtf8());
	}

	return ProcessCallback(params, request);
}


// private methods

imtrest::ConstResponsePtr CExternalLoginCallbackServletComp::ProcessCallback(
			const imtrest::IRequest::CommandParams& params,
			const imtrest::CHttpRequest& request) const
{
	const QByteArray state = params.value(QByteArrayLiteral("state"));
	const QByteArray code = params.value(QByteArrayLiteral("code"));
	const QString providerError = QString::fromUtf8(params.value(QByteArrayLiteral("error")));

	if (state.isEmpty()){
		return CreatePageResponse(false, QStringLiteral("The sign-in response is incomplete. Please start the sign-in again."), request);
	}

	QString message;
	const bool isSuccessful = m_callbackHandlerCompPtr->HandleCallback(state, code, providerError, message);

	return CreatePageResponse(isSuccessful, message, request);
}


imtrest::ConstResponsePtr CExternalLoginCallbackServletComp::CreatePageResponse(
			bool isSuccessful,
			const QString& message,
			const imtrest::CHttpRequest& request) const
{
	const QString title = isSuccessful ? QStringLiteral("Signed in") : QStringLiteral("Sign-in failed");

	const QString page = QStringLiteral(
				"<!DOCTYPE html><html><head><meta charset=\"utf-8\">"
				"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
				"<title>%1</title>"
				"<style>body{font-family:'Segoe UI',sans-serif;margin:0;display:flex;align-items:center;justify-content:center;min-height:100vh;background:#f5f5f5;color:#202020}"
				"main{max-width:420px;padding:32px;background:#fff;border:1px solid #ddd;border-radius:8px;text-align:center}"
				"h1{font-size:20px;margin:0 0 12px}p{margin:0;line-height:1.5}</style>"
				"</head><body><main><h1>%1</h1><p>%2</p></main></body></html>")
				.arg(title.toHtmlEscaped(), message.toHtmlEscaped());

	imtrest::ConstResponsePtr retVal;
	retVal.SetPtr(request.GetProtocolEngine().CreateResponse(
				request,
				imtrest::IProtocolEngine::SC_OK,
				page.toUtf8(),
				QByteArrayLiteral("text/html; charset=utf-8")).PopInterfacePtr());

	return retVal;
}


} // namespace imtauthgql


