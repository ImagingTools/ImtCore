// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CLiveTestBase.h"


// Qt includes
#include <QtCore/QJsonDocument>

// ImtCore includes
#include <imtrestdemo/CDemoProtocol.h>


void CLiveTestBase::initTestCase()
{
	QVERIFY(m_directory.isValid());

	imtrestdemo::CDemoServerSettings settings;
	settings.filesDirectory = m_directory.path();
	settings.requestInactivityTimeoutMs = s_requestInactivityTimeoutMs;
	settings.keepAliveTimeoutMs = s_keepAliveTimeoutMs;
	settings.requestProcessingTimeoutMs = s_requestProcessingTimeoutMs;
	CustomizeSettings(settings);

	QVERIFY2(m_server.Start(settings), qPrintable(m_server.GetErrorString()));
	QVERIFY(m_server.GetPort() > 0);
}


void CLiveTestBase::CustomizeSettings(imtrestdemo::CDemoServerSettings& /*settings*/)
{
}


void CLiveTestBase::cleanupTestCase()
{
	m_server.Stop();
}


int CLiveTestBase::GetPort() const
{
	return m_server.GetPort();
}


QString CLiveTestBase::GetFilesDirectory() const
{
	return m_server.GetFilesDirectory();
}


QString CLiveTestBase::GetUploadDirectory() const
{
	return m_server.GetUploadDirectory();
}


CLiveTestBase::Headers CLiveTestBase::WithToken(const Headers& headers)
{
	Headers retVal = headers;
	retVal.append({"Authorization", QByteArray("Bearer ") + imtrestdemo::DemoProtocol::s_uploadToken});

	return retVal;
}


bool CLiveTestBase::Exchange(const QByteArray& request, Response& response, bool isHeadRequest, int timeoutMs)
{
	CRawHttpClient client;

	return Connect(client) && client.Send(request) && client.ReadResponse(response, isHeadRequest, timeoutMs);
}


bool CLiveTestBase::Request(
			const QByteArray& method,
			const QByteArray& target,
			Response& response,
			const Headers& headers,
			const QByteArray& body,
			int timeoutMs)
{
	return Exchange(CRawHttpClient::CreateRequest(method, target, headers, body), response, method == "HEAD", timeoutMs);
}


bool CLiveTestBase::Get(const QByteArray& target, Response& response, const Headers& headers)
{
	return Request("GET", target, response, headers);
}


bool CLiveTestBase::Connect(CRawHttpClient& client)
{
	return client.Connect(GetPort());
}


QJsonObject CLiveTestBase::ParseJson(const QByteArray& data)
{
	return QJsonDocument::fromJson(data).object();
}


bool CLiveTestBase::IsServerAlive()
{
	Response response;

	return Get("/echo", response) && response.statusCode == 200;
}
