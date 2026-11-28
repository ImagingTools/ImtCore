// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtNetwork/QTcpServer>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>


/**
	Tests of the outgoing HTTP request (imtrest::CHttpClientRequest) and its synchronous sender
	(imtrest::CHttpClientRequestSender) against a local server.
*/
class CHttpClientRequestTest: public QObject
{
	Q_OBJECT

private slots:
	void init();
	void cleanup();

	// Data model
	void DefaultsTest();
	void ReadyStateTest();
	void CopyAndResetTest();

	// Sending
	void PostReplyTest();
	void GetReplyTest();
	void HttpErrorStatusTest();
	void TimeoutTest();
	void CancelTest();
	void PreCanceledReusedRequestClearsReplyTest();
	void TooLargeReplyTest();
	void InvalidMaxReplySizeTest();
	void InvalidUrlTest();
	void ConnectionRefusedTest();

private:
	void StartServer(const QByteArray& response, bool shouldAnswer = true);
	QUrl GetServerUrl(const QString& path) const;

private:
	QTcpServer* m_serverPtr = nullptr;
	QByteArray m_response;
	bool m_shouldAnswer = true;
	QByteArray m_receivedRequest;
};
