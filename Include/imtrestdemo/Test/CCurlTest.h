// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QTemporaryDir>
#include <QtTest/QtTest>

// ImtCore includes
#include <imtrestdemo/CDemoServerThread.h>
#include "DemoTestServer.h"


/**
	Live server: the same features as the tests CLive*, but the requests are sent by the program curl, which is an independent
	implementation of HTTP: it is how the server is used by the scripts and by the people.

	The test is skipped if there is no curl. The program is searched in this order:
	- the argument of the test executable \c --curl <path>;
	- the environment variable \c IMTREST_CURL;
	- the directories of \c PATH.
*/
class CCurlTest: public QObject
{
	Q_OBJECT
private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();

	void EchoTest();
	void MethodsTest_data();
	void MethodsTest();
	void KeepAliveTest();
	void HeadTest();
	void Http10Test();

	void FileDownloadTest();
	void RangeTest_data();
	void RangeTest();
	void ResumeTest();
	void ConditionsTest();
	void PrivateFileTest();
	void PathSafetyTest_data();
	void PathSafetyTest();

	void UploadTest();
	void UploadChunkedTest();
	void UploadWithoutTokenTest();
	void ExpectContinueTest();
	void BodyLimitTest();
	void RefusedPathTest();

	void StreamTest_data();
	void StreamTest();
	void StreamHeadTest();

	void StatusTest_data();
	void StatusTest();
	void RedirectTest();
	void ServletExceptionTest();
	void ProcessingTimeoutTest();
	void ParallelTest();

private:
	struct CurlResult
	{
		int exitCode = -1;
		int statusCode = 0;
		QMap<QByteArray, QByteArray> headers;
		QByteArray headerDump;
		QByteArray body;
		QByteArray standardOutput;
		QByteArray standardError;

		bool IsOk() const
		{
			return exitCode == 0;
		}
	};

	/**
		Run curl with the arguments, the URL is the last one. The headers and the body of the last response are read from the files.
	*/
	CurlResult RunCurl(const QStringList& arguments, const QString& path, int timeoutMs = 60000);

	/**
		Run curl with exactly the arguments (to see its output).
	*/
	CurlResult RunRawCurl(const QStringList& arguments, int timeoutMs = 60000);

	QString GetUrl(const QString& path) const;
	QString CreateTempPath(const QString& name) const;
	QString CreateFile(const QString& name, const QByteArray& content) const;
	static QByteArray ReadFile(const QString& path);
	static QStringList WithToken(const QStringList& arguments = QStringList());
	static QJsonObject ParseJson(const QByteArray& data);

	static QString FindCurl();

private:
	imtrestdemo::CDemoServerThread m_server{CreateTestCompositionFactory()};
	QTemporaryDir m_serverDirectory;
	QTemporaryDir m_workDirectory;
	QString m_curlPath;
};


