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

	QJsonObject replyObject = QJsonObject({
		std::make_pair("Code", errorCode),
		std::make_pair("ErrorString", errorString),
		std::make_pair("AdditionalInfo", additionalInfo),
	});
	QByteArray replyPayload = QJsonDocument(replyObject).toJson(QJsonDocument::Compact);

	retVal.reset(request.GetProtocolEngine().CreateResponse(
		request,
		protocolStatusCode,
		replyPayload,
		QByteArray("application/json; charset=utf-8")));

	return retVal;
}


} // namespace imtrest
