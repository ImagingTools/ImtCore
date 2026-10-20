// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>


class CHttpStreamingResponseSenderTest: public QObject
{
	Q_OBJECT
private slots:
	// The framing of the body
	void KnownSizeTest();
	void ChunkedTest();
	void UntilCloseTest();
	void ConnectionClosingHintTest();

	// The response without a body: the source is not created
	void NoBodyTest_data();
	void NoBodyTest();

	// The response cannot be sent
	void NotStreamingResponseTest();
	void UnknownStatusTest();
	void NoSourceTest();
	void ClosedSocketTest();
	void ClosingConnectionTest();
	void BusySenderTest();

	// The end of the response
	void HandlerIsCalledOnceTest();
	void HandlerIsCalledOnSourceErrorTest();
	void NextResponseAfterFinishTest();
};


