// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtNetwork/QNetworkReply>

// Local includes
#include "CLiveTestBase.h"


/**
	Live server: the bodies of the responses that are sent by parts (step 1.9 of imtrest): the size that is announced and that is not,
	the slow producer, the break of the connection by the client, and the work with a real client library (QNetworkAccessManager).
*/
class CLiveStreamTest: public CLiveTestBase
{
	Q_OBJECT
private Q_SLOTS:
	// 1.9: the streaming responses
	void StreamTest();
	void StreamZeroTest();
	void StreamChunkedTest();
	void StreamChunkedSmallPartsTest();
	void StreamHttp10Test();
	void StreamHeadTest();
	void StreamBadSizeTest_data();
	void StreamBadSizeTest();
	void SlowStreamTest();
	void SlowReaderTest();
	void StreamAbortTest();
	void BigBufferedBodyTest();
	void TooBigBufferedBodyTest();

	// A client library that is not written for the test
	void NetworkGetTest();
	void NetworkPostTest();
	void NetworkUploadDownloadTest();
	void NetworkRangeTest();
	void NetworkStreamTest_data();
	void NetworkStreamTest();
	void NetworkStatusTest_data();
	void NetworkStatusTest();
	void NetworkConditionTest();
	void NetworkRedirectTest();
	void NetworkKeepAliveTest();

private:
	/**
		Wait for the end of the reply (the loop of events of the test works, the server is in its thread).
	*/
	static bool WaitForReply(QNetworkReply& reply, int timeoutMs = 20000);

	QUrl CreateUrl(const QString& pathAndQuery) const;
	QNetworkRequest CreateRequest(const QString& pathAndQuery) const;
};


