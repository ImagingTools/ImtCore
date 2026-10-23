// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CLiveSettingsTest.h"


// Qt includes
#include <QtCore/QElapsedTimer>
#include <QtCore/QJsonDocument>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QTcpServer>

// ACF includes
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtrestdemo/CDemoProtocol.h>


namespace
{


const char* const s_baseToken = imtrestdemo::DemoProtocol::s_uploadToken;


}


void CLiveSettingsTest::init()
{
	if (m_port == 0){
		m_basePort = GetPort();
		m_port = m_basePort;
		m_token = s_baseToken;
	}
}


void CLiveSettingsTest::cleanup()
{
	// The server gets the settings that it had at the start (the state is read with the token and the port that the test has set)
	QJsonObject changes;
	changes.insert("port", m_basePort);
	changes.insert("token", QString::fromLatin1(s_baseToken));
	changes.insert("keepAliveTimeoutMs", int(s_keepAliveTimeoutMs));
	changes.insert("requestInactivityTimeoutMs", int(s_requestInactivityTimeoutMs));
	changes.insert("requestProcessingTimeoutMs", int(s_requestProcessingTimeoutMs));
	changes.insert("threadsLimit", 16);
	changes.insert("maxBodySize", 0);

	// An earlier restart must be finished, otherwise the changes of the test are lost between the restarts
	QVERIFY(WaitForSettings([](const QJsonObject& settings){
		return !settings.value("isRestartPending").toBool();
	}));

	Response response;
	QVERIFY(PutSettings(changes, response));
	QCOMPARE(response.statusCode, 200);

	m_port = m_basePort;
	m_token = s_baseToken;

	QVERIFY(WaitForSettings([this](const QJsonObject& settings){
		return !settings.value("isRestartPending").toBool()
					&& settings.value("port").toInt() == m_basePort
					&& settings.value("keepAliveTimeoutMs").toInt() == s_keepAliveTimeoutMs
					&& settings.value("token").toString() == QString::fromLatin1(s_baseToken);
	}));
}


void CLiveSettingsTest::AccessNeedsTokenTest()
{
	const int restartCount = GetSettings().value("restartCount").toInt();

	Response response;

	// Reading and changing are refused before the body is read
	QVERIFY(Call("GET", QByteArray(), response, QByteArray()));
	QCOMPARE(response.statusCode, 401);
	QVERIFY(response.HasHeader("www-authenticate"));

	QVERIFY(Call("PUT", "{\"threadsLimit\": 1}", response, QByteArray()));
	QCOMPARE(response.statusCode, 401);

	QVERIFY(Call("GET", QByteArray(), response, "wrong-token"));
	QCOMPARE(response.statusCode, 401);

	// Nothing is changed
	const QJsonObject settings = GetSettings();
	QCOMPARE(settings.value("threadsLimit").toInt(), 16);
	QCOMPARE(settings.value("restartCount").toInt(), restartCount);
}


void CLiveSettingsTest::GetSettingsTest()
{
	Response response;
	QVERIFY(Call("GET", QByteArray(), response, m_token));
	QCOMPARE(response.statusCode, 200);
	QVERIFY(response.GetHeader("content-type").startsWith("application/json"));

	const QJsonObject settings = ParseJson(response.body);
	QCOMPARE(settings.value("port").toInt(), m_port);
	QCOMPARE(settings.value("token").toString(), QString::fromLatin1(s_baseToken));
	QCOMPARE(settings.value("filesDirectory").toString(), GetFilesDirectory());
	QCOMPARE(settings.value("threadsLimit").toInt(), 16);
	QCOMPARE(settings.value("requestInactivityTimeoutMs").toInt(), int(s_requestInactivityTimeoutMs));
	QCOMPARE(settings.value("keepAliveTimeoutMs").toInt(), int(s_keepAliveTimeoutMs));
	QCOMPARE(settings.value("requestProcessingTimeoutMs").toInt(), int(s_requestProcessingTimeoutMs));
	QCOMPARE(settings.value("writeHighWatermark").toInt(), 1048576);
	QCOMPARE(settings.value("maxBodySize").toInt(), 0);
	QVERIFY(settings.contains("isLogEnabled"));
	QCOMPARE(settings.value("isRestartPending").toBool(), false);
	QCOMPARE(settings.value("lastError").toString(), QString());

	// Only /admin/settings exists
	Response otherResponse;
	CRawHttpClient client;
	QVERIFY(client.Connect(m_port));
	QVERIFY(client.Send(CRawHttpClient::CreateRequest("GET", "/admin/other", {{"Authorization", "Bearer " + m_token}})));
	QVERIFY(client.ReadResponse(otherResponse));
	QCOMPARE(otherResponse.statusCode, 404);
}


void CLiveSettingsTest::InvalidChangesTest_data()
{
	QTest::addColumn<QByteArray>("body");

	QTest::newRow("not json") << QByteArray("threadsLimit=1");
	QTest::newRow("array") << QByteArray("[1, 2]");
	QTest::newRow("empty body") << QByteArray();
	QTest::newRow("text instead of number") << QByteArray("{\"threadsLimit\": \"4\"}");
	QTest::newRow("fraction") << QByteArray("{\"threadsLimit\": 1.5}");
	QTest::newRow("threads are zero") << QByteArray("{\"threadsLimit\": 0}");
	QTest::newRow("too many threads") << QByteArray("{\"threadsLimit\": 100000}");
	QTest::newRow("negative timeout") << QByteArray("{\"keepAliveTimeoutMs\": -1}");
	QTest::newRow("port zero") << QByteArray("{\"port\": 0}");
	QTest::newRow("port too big") << QByteArray("{\"port\": 70000}");
	QTest::newRow("tiny watermark") << QByteArray("{\"writeHighWatermark\": 10}");
	QTest::newRow("negative body limit") << QByteArray("{\"maxBodySize\": -5}");
	QTest::newRow("token with a space") << QByteArray("{\"token\": \"two words\"}");
	QTest::newRow("empty token") << QByteArray("{\"token\": \"\"}");
	QTest::newRow("token is a number") << QByteArray("{\"token\": 5}");
	QTest::newRow("log is a text") << QByteArray("{\"isLogEnabled\": \"yes\"}");
	QTest::newRow("directory cannot be changed") << QByteArray("{\"filesDirectory\": \"/tmp\"}");
	QTest::newRow("unknown setting") << QByteArray("{\"unknown\": 1}");

	// One wrong value cancels all changes of the request
	QTest::newRow("good and bad") << QByteArray("{\"keepAliveTimeoutMs\": 5000, \"threadsLimit\": 0}");
}


void CLiveSettingsTest::InvalidChangesTest()
{
	QFETCH(QByteArray, body);

	const int restartCount = GetSettings().value("restartCount").toInt();

	Response response;
	QVERIFY(Call("PUT", body, response, m_token));
	QCOMPARE(response.statusCode, 400);

	const QJsonObject error = ParseJson(response.body);
	QVERIFY2(!error.value("error").toString().isEmpty(), response.body.constData());

	// Nothing is changed and nothing is going to restart
	QTest::qSleep(500);
	const QJsonObject settings = GetSettings();
	QCOMPARE(settings.value("isRestartPending").toBool(), false);
	QCOMPARE(settings.value("restartCount").toInt(), restartCount);
	QCOMPARE(settings.value("keepAliveTimeoutMs").toInt(), int(s_keepAliveTimeoutMs));
	QCOMPARE(settings.value("threadsLimit").toInt(), 16);
}


void CLiveSettingsTest::SameValuesDoNotRestartTest()
{
	const int restartCount = GetSettings().value("restartCount").toInt();

	QJsonObject changes;
	changes.insert("keepAliveTimeoutMs", int(s_keepAliveTimeoutMs));
	changes.insert("threadsLimit", 16);

	Response response;
	QVERIFY(PutSettings(changes, response));
	QCOMPARE(response.statusCode, 200);
	QCOMPARE(ParseJson(response.body).value("restartScheduled").toBool(), false);

	// An empty object is a valid request that changes nothing
	QVERIFY(Call("PUT", "{}", response, m_token));
	QCOMPARE(response.statusCode, 200);
	QCOMPARE(ParseJson(response.body).value("restartScheduled").toBool(), false);

	QTest::qSleep(700);
	QCOMPARE(GetSettings().value("restartCount").toInt(), restartCount);
}


void CLiveSettingsTest::TimeoutsAreChangedTest()
{
	// The idle connection is closed after the time of the settings: 1500 ms at the start
	auto measureIdleClose = [this](qint64& elapsedMs){
		CRawHttpClient client;
		Response response;
		if (!client.Connect(m_port) || !client.Send(CRawHttpClient::CreateRequest("GET", "/echo")) || !client.ReadResponse(response) || response.statusCode != 200){
			return false;
		}

		QElapsedTimer timer;
		timer.start();
		if (!client.WaitForClose(5000)){
			return false;
		}

		elapsedMs = timer.elapsed();

		return true;
	};

	const int restartCount = GetSettings().value("restartCount").toInt();

	qint64 before = 0;
	QVERIFY(measureIdleClose(before));
	QVERIFY2(before >= 1000, qPrintable(QString::number(before)));

	QJsonObject changes;
	changes.insert("keepAliveTimeoutMs", 300);
	changes.insert("threadsLimit", 4);
	changes.insert("maxBodySize", 100000);
	changes.insert("writeHighWatermark", 65536);
	changes.insert("isLogEnabled", false);

	Response response;
	QVERIFY(PutSettings(changes, response));
	QCOMPARE(response.statusCode, 200);

	const QJsonObject result = ParseJson(response.body);
	QCOMPARE(result.value("restartScheduled").toBool(), true);
	QCOMPARE(result.value("port").toInt(), m_port);

	QJsonObject settings;
	QVERIFY(WaitForSettings([](const QJsonObject& current){
		return current.value("keepAliveTimeoutMs").toInt() == 300 && !current.value("isRestartPending").toBool();
	}, &settings));

	QCOMPARE(settings.value("threadsLimit").toInt(), 4);
	QCOMPARE(settings.value("maxBodySize").toInt(), 100000);
	QCOMPARE(settings.value("writeHighWatermark").toInt(), 65536);
	QCOMPARE(settings.value("restartCount").toInt(), restartCount + 1);
	QCOMPARE(settings.value("lastError").toString(), QString());

	// The settings that are not in the request are not changed
	QCOMPARE(settings.value("requestInactivityTimeoutMs").toInt(), int(s_requestInactivityTimeoutMs));
	QCOMPARE(settings.value("token").toString(), QString::fromLatin1(s_baseToken));

	// The server works by the new settings: the same connection is closed quicker
	qint64 after = 0;
	QVERIFY(measureIdleClose(after));
	QVERIFY2(after < 1000, qPrintable(QString::number(after)));

	// /info shows the same
	Response infoResponse;
	QVERIFY(Call("GET", QByteArray(), infoResponse, m_token));
	CRawHttpClient client;
	QVERIFY(client.Connect(m_port));
	QVERIFY(client.Send(CRawHttpClient::CreateRequest("GET", "/info")));
	QVERIFY(client.ReadResponse(infoResponse));
	const QJsonObject info = ParseJson(infoResponse.body);
	QCOMPARE(info.value("keepAliveTimeoutMs").toInt(), 300);
	QCOMPARE(info.value("threadsLimit").toInt(), 4);
	QCOMPARE(info.value("maxBodySize").toInt(), 100000);
}


void CLiveSettingsTest::TokenIsChangedTest()
{
	const QByteArray newToken = "another.token-2";

	QJsonObject changes;
	changes.insert("token", QString::fromLatin1(newToken));

	Response response;
	QVERIFY(PutSettings(changes, response));
	QCOMPARE(response.statusCode, 200);
	QCOMPARE(ParseJson(response.body).value("token").toString(), QString::fromLatin1(newToken));

	// The server is restarted with the new token, the old one is not accepted any more
	m_token = newToken;
	QVERIFY(WaitForSettings([&newToken](const QJsonObject& current){
		return current.value("token").toString() == QString::fromLatin1(newToken) && !current.value("isRestartPending").toBool();
	}));

	QVERIFY(Call("GET", QByteArray(), response, s_baseToken));
	QCOMPARE(response.statusCode, 401);

	// The other paths that need the token follow it
	QVERIFY(ServerRequest("PUT", "/upload/by-new-token.bin", response, {{"Authorization", "Bearer " + QByteArray(s_baseToken)}}, "1"));
	QCOMPARE(response.statusCode, 401);

	QVERIFY(ServerRequest("PUT", "/upload/by-new-token.bin", response, {{"Authorization", "Bearer " + newToken}}, "123"));
	QCOMPARE(response.statusCode, 201);

	QVERIFY(ServerRequest("GET", "/files/private/secret.txt", response, {{"Authorization", "Bearer " + newToken}}));
	QCOMPARE(response.statusCode, 200);
}


void CLiveSettingsTest::PortIsChangedTest()
{
	const int newPort = FindFreePort();
	QVERIFY(newPort > 0 && newPort != m_port);

	QJsonObject changes;
	changes.insert("port", newPort);

	Response response;
	QVERIFY(PutSettings(changes, response));
	QCOMPARE(response.statusCode, 200);
	QCOMPARE(ParseJson(response.body).value("port").toInt(), newPort);

	const int oldPort = m_port;
	m_port = newPort;
	QJsonObject settings;
	QVERIFY(WaitForSettings([newPort](const QJsonObject& current){
		return current.value("port").toInt() == newPort && !current.value("isRestartPending").toBool();
	}, &settings));
	QCOMPARE(settings.value("lastError").toString(), QString());

	// Nobody listens to the old port
	CRawHttpClient oldClient;
	QVERIFY(!oldClient.Connect(oldPort, 1000));

	QVERIFY(Call("GET", QByteArray(), response, m_token));
	QCOMPARE(response.statusCode, 200);

	QVERIFY(ServerRequest("GET", "/echo", response));
	QCOMPARE(response.statusCode, 200);
}


void CLiveSettingsTest::BusyPortKeepsTheServerTest()
{
	// The port that is taken by someone: the server cannot use it and goes back to the previous settings
	const int restartCount = GetSettings().value("restartCount").toInt();

	QTcpServer busyServer;
	QVERIFY(busyServer.listen(QHostAddress::Any, 0));
	const int busyPort = busyServer.serverPort();

	QJsonObject changes;
	changes.insert("port", busyPort);
	changes.insert("keepAliveTimeoutMs", 777);

	Response response;
	QVERIFY(PutSettings(changes, response));
	QCOMPARE(response.statusCode, 200);
	QCOMPARE(ParseJson(response.body).value("restartScheduled").toBool(), true);

	QJsonObject settings;
	QVERIFY(WaitForSettings([restartCount](const QJsonObject& current){
		return current.value("restartCount").toInt() > restartCount && !current.value("isRestartPending").toBool();
	}, &settings));

	// The server works on the old port with the old settings, the reason is shown
	QCOMPARE(settings.value("port").toInt(), m_port);
	QCOMPARE(settings.value("keepAliveTimeoutMs").toInt(), int(s_keepAliveTimeoutMs));
	QVERIFY2(settings.value("lastError").toString().contains(QString::number(busyPort)), qPrintable(settings.value("lastError").toString()));

	QVERIFY(ServerRequest("GET", "/echo", response));
	QCOMPARE(response.statusCode, 200);

	// The next successful change clears the error
	QJsonObject nextChanges;
	nextChanges.insert("threadsLimit", 8);
	QVERIFY(PutSettings(nextChanges, response));
	QCOMPARE(response.statusCode, 200);
	QVERIFY(WaitForSettings([](const QJsonObject& current){
		return current.value("threadsLimit").toInt() == 8 && !current.value("isRestartPending").toBool();
	}, &settings));
	QCOMPARE(settings.value("lastError").toString(), QString());
}


void CLiveSettingsTest::QuickChangesAreMergedTest()
{
	// The second request comes while the restart waits: the changes of both requests are used and the server restarts once
	const int restartCount = GetSettings().value("restartCount").toInt();

	QJsonObject first;
	first.insert("keepAliveTimeoutMs", 900);

	QJsonObject second;
	second.insert("threadsLimit", 5);

	Response response;
	QVERIFY(PutSettings(first, response));
	QCOMPARE(response.statusCode, 200);
	QVERIFY(PutSettings(second, response));
	QCOMPARE(response.statusCode, 200);
	QCOMPARE(ParseJson(response.body).value("restartScheduled").toBool(), true);

	QJsonObject settings;
	QVERIFY(WaitForSettings([](const QJsonObject& current){
		return current.value("threadsLimit").toInt() == 5 && !current.value("isRestartPending").toBool();
	}, &settings));

	QCOMPARE(settings.value("keepAliveTimeoutMs").toInt(), 900);
	QCOMPARE(settings.value("restartCount").toInt(), restartCount + 1);
}


void CLiveSettingsTest::WholeServerWorksAfterRestartTest()
{
	QJsonObject changes;
	changes.insert("requestProcessingTimeoutMs", 2500);

	Response response;
	QVERIFY(PutSettings(changes, response));
	QCOMPARE(response.statusCode, 200);

	QVERIFY(WaitForSettings([](const QJsonObject& current){
		return current.value("requestProcessingTimeoutMs").toInt() == 2500 && !current.value("isRestartPending").toBool();
	}));

	// All services are on their places, with the files that were before the restart
	QVERIFY(ServerRequest("GET", "/files/hello.txt", response));
	QCOMPARE(response.statusCode, 200);
	QCOMPARE(response.body, QByteArray(imtrestdemo::DemoProtocol::s_helloFileText));

	QVERIFY(ServerRequest("GET", "/stream/100000", response));
	QCOMPARE(response.statusCode, 200);
	QCOMPARE(response.body.size(), qsizetype(100000));

	QVERIFY(ServerRequest("POST", "/echo", response, Headers(), "body"));
	QCOMPARE(response.statusCode, 200);
	QCOMPARE(ParseJson(response.body).value("body").toString(), QString("body"));

	QVERIFY(ServerRequest("PUT", "/upload/after-restart.bin", response, {{"Authorization", "Bearer " + m_token}}, QByteArray(5000, 'x')));
	QCOMPARE(response.statusCode, 201);

	QVERIFY(ServerRequest("GET", "/refuse", response));
	QCOMPARE(response.statusCode, 403);

	QVERIFY(ServerRequest("GET", "/behavior/status/204", response));
	QVERIFY(response.statusCode == 204 || response.statusCode == 200);
}


// private methods

bool CLiveSettingsTest::Call(const QByteArray& method, const QByteArray& body, Response& response, const QByteArray& token) const
{
	Headers headers;
	if (!token.isEmpty()){
		headers.append(qMakePair(QByteArray("Authorization"), "Bearer " + token));
	}

	if (!body.isEmpty()){
		headers.append(qMakePair(QByteArray("Content-Type"), QByteArray("application/json")));
	}

	CRawHttpClient client;

	return client.Connect(m_port) && client.Send(CRawHttpClient::CreateRequest(method, "/admin/settings", headers, body)) && client.ReadResponse(response);
}


bool CLiveSettingsTest::ServerRequest(const QByteArray& method, const QByteArray& target, Response& response, const Headers& headers, const QByteArray& body) const
{
	CRawHttpClient client;

	return client.Connect(m_port) && client.Send(CRawHttpClient::CreateRequest(method, target, headers, body)) && client.ReadResponse(response, method == "HEAD");
}


bool CLiveSettingsTest::PutSettings(const QJsonObject& changes, Response& response) const
{
	return Call("PUT", QJsonDocument(changes).toJson(QJsonDocument::Compact), response, m_token);
}


QJsonObject CLiveSettingsTest::GetSettings() const
{
	Response response;
	if (!Call("GET", QByteArray(), response, m_token) || response.statusCode != 200){
		return QJsonObject();
	}

	return ParseJson(response.body);
}


bool CLiveSettingsTest::WaitForSettings(const Predicate& isReady, QJsonObject* settingsPtr, int timeoutMs) const
{
	QElapsedTimer timer;
	timer.start();

	while (timer.elapsed() < timeoutMs){
		const QJsonObject settings = GetSettings();
		if (!settings.isEmpty() && isReady(settings)){
			if (settingsPtr != nullptr){
				*settingsPtr = settings;
			}

			return true;
		}

		QTest::qSleep(50);
	}

	return false;
}


int CLiveSettingsTest::FindFreePort()
{
	QTcpServer probe;
	if (!probe.listen(QHostAddress::Any, 0)){
		return 0;
	}

	return probe.serverPort();
}


I_ADD_TEST(CLiveSettingsTest);

