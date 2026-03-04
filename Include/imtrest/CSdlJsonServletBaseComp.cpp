// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CSdlJsonServletBaseComp.h>


namespace imtrest
{


CSdlJsonServletBaseComp::ResponsePtr CSdlJsonServletBaseComp::CreateDefaultErrorResponse(
			int errorCode,
			const QString& errorString,
			const QString& additionalInfo,
			IProtocolEngine::StatusCode protocolStatusCode,
			const imtrest::CHttpRequest& request) const
{
	ResponsePtr retVal;

	QJsonObject replyObject;
	replyObject["Code"] = errorCode;
	replyObject["ErrorString"] = errorString;
	replyObject["AdditionalInfo"] = additionalInfo;

	QByteArray replyPayload = QJsonDocument(replyObject).toJson(QJsonDocument::Compact);

	retVal.reset(request.GetProtocolEngine().CreateResponse(
		request,
		protocolStatusCode,
		replyPayload,
		QByteArray("application/json; charset=utf-8")).PopInterfacePtr());

	return retVal;
}


} // namespace imtrest
