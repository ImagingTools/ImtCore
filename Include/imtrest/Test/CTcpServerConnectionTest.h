// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>


/**
	Tests of the connections of the server on the real components (CTcpServerComp, CHttpProtocolEngineComp) and real sockets:
	the limit of the read buffer while a request is handled (CSocket) and the connections that wait for a free thread (CMultiThreadServer).
*/
class CTcpServerConnectionTest: public QObject
{
	Q_OBJECT
private slots:
	// The data of the next request must not pile up in the memory of the server while the current request is handled
	void ReadBufferIsLimitedWhileRequestIsHandledTest();
	void NextRequestIsReceivedAfterResponseTest();

	// After the response the socket is read together with the parsing of the request: the data do not pile up in the read buffer
	void SocketIsReadAsFastAsBodyIsReceivedTest();

	// The pipelined requests must not pile up their responses in the memory while a slow client does not read the previous one
	void PipelinedRequestsWaitForResponseToBeDeliveredTest();

	// More connections than threads: the connections wait in the queue and are served one after another
	void QueuedConnectionsAreServedTest();
};


