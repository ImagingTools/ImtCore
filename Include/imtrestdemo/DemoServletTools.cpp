// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrestdemo/DemoServletTools.h>


// STL includes
#include <atomic>
#include <memory>

// Qt includes
#include <QtCore/QJsonDocument>

// ImtCore includes
#include <imtrest/CHttpResponse.h>


namespace imtrestdemo
{


namespace DemoServletTools
{


namespace
{
std::atomic<int> s_serverStopCount(0);
}


void NotifyServerStopping()
{
	++s_serverStopCount;
}


int GetServerStopCount()
{
	return s_serverStopCount.load();
}


imtrest::ConstResponsePtr CreateResponse(
			const imtrest::IProtocolEngine& engine,
			int statusCode,
			const QByteArray& data,
			const QByteArray& dataTypeId,
			const imtrest::IResponse::Headers& headers)
{
	std::unique_ptr<imtrest::CHttpResponse> responsePtr(new imtrest::CHttpResponse(statusCode, data, dataTypeId, engine));
	responsePtr->SetHeaders(headers);

	return imtrest::ConstResponsePtr(responsePtr.release());
}


imtrest::ConstResponsePtr CreateJsonResponse(
			const imtrest::IProtocolEngine& engine,
			int statusCode,
			const QJsonObject& body,
			const imtrest::IResponse::Headers& headers)
{
	return CreateResponse(engine, statusCode, QJsonDocument(body).toJson(QJsonDocument::Indented), "application/json", headers);
}


imtrest::ConstResponsePtr CreateTextResponse(const imtrest::IProtocolEngine& engine, int statusCode, const QString& text)
{
	return CreateResponse(engine, statusCode, text.toUtf8(), "text/plain; charset=utf-8");
}


QByteArray GetMethodName(imtrest::CHttpRequest::MethodType method)
{
	switch (method){
	case imtrest::CHttpRequest::MT_GET:
		return "GET";
	case imtrest::CHttpRequest::MT_PUT:
		return "PUT";
	case imtrest::CHttpRequest::MT_POST:
		return "POST";
	case imtrest::CHttpRequest::MT_DELETE:
		return "DELETE";
	case imtrest::CHttpRequest::MT_HEAD:
		return "HEAD";
	case imtrest::CHttpRequest::MT_OPTIONS:
		return "OPTIONS";
	case imtrest::CHttpRequest::MT_PATCH:
		return "PATCH";
	default:
		return "UNKNOWN";
	}
}


QByteArrayList GetPathArguments(const imtrest::CHttpRequest& request)
{
	QByteArrayList segments = request.GetCommandId().split('/');
	segments.removeAll(QByteArray());

	if (!segments.isEmpty()){
		segments.removeFirst();
	}

	return segments;
}


} // namespace DemoServletTools


} // namespace imtrestdemo
