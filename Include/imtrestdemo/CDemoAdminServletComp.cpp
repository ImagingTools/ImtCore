// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrestdemo/CDemoAdminServletComp.h>


// Qt includes
#include <QtCore/QJsonDocument>

// ImtCore includes
#include <imtrestdemo/DemoServletTools.h>


namespace imtrestdemo
{


namespace
{


const QByteArray s_settingsCommandId = "/admin/settings";


imtrest::ConstResponsePtr CreateError(const imtrest::CHttpRequest& request, int statusCode, const QString& text)
{
	QJsonObject body;
	body.insert("error", text);

	return DemoServletTools::CreateJsonResponse(request.GetProtocolEngine(), statusCode, body);
}


} // namespace


// protected methods

// reimplemented (imtrest::CHttpServletCompBase)

imtrest::ConstResponsePtr CDemoAdminServletComp::OnGet(
			const QByteArray& commandId,
			const imtrest::IRequest::CommandParams& /*commandParams*/,
			const HeadersMap& /*headers*/,
			const imtrest::CHttpRequest& request) const
{
	if (commandId != s_settingsCommandId){
		return CreateError(request, imtrest::IProtocolEngine::SC_NOT_FOUND, QStringLiteral("There is only admin/settings"));
	}

	return DemoServletTools::CreateJsonResponse(request.GetProtocolEngine(), imtrest::IProtocolEngine::SC_OK, m_settingsControllerCompPtr->GetSettings());
}


imtrest::ConstResponsePtr CDemoAdminServletComp::OnPut(
			const QByteArray& commandId,
			const imtrest::IRequest::CommandParams& /*commandParams*/,
			const HeadersMap& /*headers*/,
			const imtrest::CHttpRequest& request) const
{
	typedef imtrest::IProtocolEngine Engine;

	if (commandId != s_settingsCommandId){
		return CreateError(request, Engine::SC_NOT_FOUND, QStringLiteral("There is only admin/settings"));
	}

	QJsonParseError parseError;
	const QJsonDocument document = QJsonDocument::fromJson(request.GetBody(), &parseError);
	if (parseError.error != QJsonParseError::NoError || !document.isObject()){
		return CreateError(request, Engine::SC_BAD_REQUEST, QStringLiteral("The body must be a JSON object with the settings"));
	}

	QString errorText;
	QJsonObject result;
	if (!m_settingsControllerCompPtr->ChangeSettings(document.object(), errorText, result)){
		return CreateError(request, Engine::SC_BAD_REQUEST, errorText);
	}

	return DemoServletTools::CreateJsonResponse(request.GetProtocolEngine(), Engine::SC_OK, result);
}


} // namespace imtrestdemo


