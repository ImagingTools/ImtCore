// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QJsonObject>
#include <QtCore/QString>

// ImtCore includes
#include <imtrest/CHttpRequest.h>
#include <imtrest/IProtocolEngine.h>
#include <imtrest/IResponse.h>


namespace imtrestdemo
{


namespace DemoServletTools
{


/**
	Tell the servlets that wait (for example the case \c sleep of \c /behavior) that a server is stopping, so they return at once:
	the workers must be finished before the components that they use are destroyed.
*/
void NotifyServerStopping();

/**
	Get the number of the stops of the servers in the process. A servlet that waits compares it with the number at its start.
*/
int GetServerStopCount();

/**
	Create a response of the HTTP engine with the headers.
*/
imtrest::ConstResponsePtr CreateResponse(
			const imtrest::IProtocolEngine& engine,
			int statusCode,
			const QByteArray& data,
			const QByteArray& dataTypeId,
			const imtrest::IResponse::Headers& headers = imtrest::IResponse::Headers());

/**
	Create a response with the JSON body.
*/
imtrest::ConstResponsePtr CreateJsonResponse(
			const imtrest::IProtocolEngine& engine,
			int statusCode,
			const QJsonObject& body,
			const imtrest::IResponse::Headers& headers = imtrest::IResponse::Headers());

/**
	Create a plain text response.
*/
imtrest::ConstResponsePtr CreateTextResponse(const imtrest::IProtocolEngine& engine, int statusCode, const QString& text);

QByteArray GetMethodName(imtrest::CHttpRequest::MethodType method);

/**
	Get the segments of the path of the request after the first one, for example \c /stream/100 gives \c ["100"].
*/
QByteArrayList GetPathArguments(const imtrest::CHttpRequest& request);


} // namespace DemoServletTools


} // namespace imtrestdemo


