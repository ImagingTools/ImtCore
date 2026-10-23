// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <memory>

// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QObject>
#include <QtCore/QTemporaryDir>
#include <QtTest/QtTest>

// ImtCore includes
#include <imtrestdemo/CDemoServerThread.h>
#include "DemoTestServer.h"

#include "CRawHttpClient.h"


class CTemporaryDirectoryOverride;


/**
	Base of the tests that work with a live demo server: the server is started for the test class (in its own thread, on a free port,
	with a temporary directory of the files) and stopped after the class. Every test connects to it as a real client.
	The server has short timeouts (see the constants) so the tests of the timeouts are fast; all other requests of the tests are far quicker.
*/
class CLiveTestBase: public QObject
{
	Q_OBJECT
public:
	static constexpr int s_requestInactivityTimeoutMs = 1000;
	static constexpr int s_keepAliveTimeoutMs = 1500;
	static constexpr int s_requestProcessingTimeoutMs = 1000;

	// The constructor and the destructor are not inline, because the member that gives the temporary directory is of a type that the header does not define
	CLiveTestBase();
	~CLiveTestBase() override;

private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();

protected:
	/**
		A class can change the settings of the server that is started for it.
	*/
	virtual void CustomizeSettings(imtrestdemo::CDemoServerSettings& settings);

	/**
		Directory that is the temporary directory of the system of the process while the tests of the class work (from the start of the
		server to its stop), empty - the real one. The components of the partitura take the temporary directory when they are created
		(the component TempLocation), and they are created when they are needed, for example at the first request, so the directory is
		given for the whole time of the class. A class can give the server a directory of its own: the files that the server writes there are
		not mixed with the files of other programs. The directory is created if it does not exist.
	*/
	virtual QString GetServerTemporaryDirectory(const imtrestdemo::CDemoServerSettings& settings) const;

	typedef CRawHttpClient::Response Response;
	typedef CRawHttpClient::Headers Headers;

	int GetPort() const;
	QString GetFilesDirectory() const;
	QString GetUploadDirectory() const;

	/**
		Authorization header with the token of the server.
	*/
	static Headers WithToken(const Headers& headers = Headers());

	/**
		Open a connection, send the request and read the response. The connection is closed after that.
	*/
	bool Exchange(const QByteArray& request, Response& response, bool isHeadRequest = false, int timeoutMs = 10000);

	/**
		Send a request created from the parts, see CRawHttpClient::CreateRequest().
	*/
	bool Request(
				const QByteArray& method,
				const QByteArray& target,
				Response& response,
				const Headers& headers = Headers(),
				const QByteArray& body = QByteArray(),
				int timeoutMs = 10000);

	bool Get(const QByteArray& target, Response& response, const Headers& headers = Headers());

	/**
		Open a connection to the server.
	*/
	bool Connect(CRawHttpClient& client);

	static QJsonObject ParseJson(const QByteArray& data);

	/**
		Check that the server still works: a new connection gets the answer.
	*/
	bool IsServerAlive();

private:
	imtrestdemo::CDemoServerThread m_server{CreateTestCompositionFactory()};
	QTemporaryDir m_directory;
	std::unique_ptr<CTemporaryDirectoryOverride> m_temporaryDirectoryOverridePtr;
};


