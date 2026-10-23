// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CLiveTestBase.h"


// STL includes
#include <vector>

// Qt includes
#include <QtCore/QDir>
#include <QtCore/QJsonDocument>

// ImtCore includes
#include <imtrestdemo/CDemoProtocol.h>


/**
	While the object lives, the temporary directory of the system of the process is the given directory: Qt reads it from the environment
	(QDir::tempPath() and QStandardPaths::TempLocation do not cache it), so the components that take it when they are created get it too.
	The previous values are restored when the object is destroyed. The object is not thread-safe, as the environment of the process is not.
*/
class CTemporaryDirectoryOverride
{
public:
	explicit CTemporaryDirectoryOverride(const QString& directory)
	{
		// Qt does not use a directory that does not exist
		QDir().mkpath(directory);

		// TMPDIR is read on Unix, TMP and TEMP on Windows
		for (const char* name : {"TMPDIR", "TMP", "TEMP"}){
			m_savedVariables.push_back({name, qEnvironmentVariableIsSet(name), qgetenv(name)});

			qputenv(name, QDir::toNativeSeparators(directory).toLocal8Bit());
		}
	}

	~CTemporaryDirectoryOverride()
	{
		for (const SVariable& variable : m_savedVariables){
			if (variable.isSet){
				qputenv(variable.name, variable.value);
			}
			else{
				qunsetenv(variable.name);
			}
		}
	}

private:
	struct SVariable
	{
		const char* name;
		bool isSet;
		QByteArray value;
	};

	std::vector<SVariable> m_savedVariables;
};


CLiveTestBase::CLiveTestBase()
{
}


CLiveTestBase::~CLiveTestBase()
{
}


void CLiveTestBase::initTestCase()
{
	QVERIFY(m_directory.isValid());

	imtrestdemo::CDemoServerSettings settings;
	settings.filesDirectory = m_directory.path();
	settings.requestInactivityTimeoutMs = s_requestInactivityTimeoutMs;
	settings.keepAliveTimeoutMs = s_keepAliveTimeoutMs;
	settings.requestProcessingTimeoutMs = s_requestProcessingTimeoutMs;
	CustomizeSettings(settings);

	// The components of the server are created when they are needed, so the directory is given until the server is stopped
	const QString temporaryDirectory = GetServerTemporaryDirectory(settings);
	if (!temporaryDirectory.isEmpty()){
		m_temporaryDirectoryOverridePtr.reset(new CTemporaryDirectoryOverride(temporaryDirectory));
	}

	QVERIFY2(m_server.Start(settings), qPrintable(m_server.GetErrorString()));
	QVERIFY(m_server.GetPort() > 0);
}


void CLiveTestBase::CustomizeSettings(imtrestdemo::CDemoServerSettings& /*settings*/)
{
}


QString CLiveTestBase::GetServerTemporaryDirectory(const imtrestdemo::CDemoServerSettings& /*settings*/) const
{
	return QString();
}


void CLiveTestBase::cleanupTestCase()
{
	m_server.Stop();

	m_temporaryDirectoryOverridePtr.reset();
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
