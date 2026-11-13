// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>


class CHttpResponseBodyStreamerTest: public QObject
{
	Q_OBJECT
private slots:
	// A complete body
	void SizedBodyTest();
	void ChunkedBodyTest();
	void UntilCloseBodyTest();
	void NonPositiveWatermarkTest();

	// Backpressure
	void SlowClientSlowsSourceDownTest();

	// A source that waits
	void WaitingSourceTest();
	void WaitingSourceIsAbortedWithStreamerTest();

	// Declared size
	void DeclaredSizeIsNotExceededTest();
	void SourceEndsBeforeDeclaredSizeTest();
	void SourceGivesMoreThanRequestedTest();

	// Failures of the source
	void SourceErrorTest();
	void SourceGivesEmptyDataTest();

	// The connection
	void ClientDisconnectAbortsSourceTest();
	void ClosedSocketAtStartTest();
	void StalledClientTest();
	void FinishedStreamerDoesNotAbortSourceTest();
};


