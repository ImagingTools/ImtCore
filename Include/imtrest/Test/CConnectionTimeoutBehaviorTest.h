// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>


class CConnectionTimeoutBehaviorTest: public QObject
{
	Q_OBJECT
private slots:
	// 408 response of HTTP
	void CreateHttpRequestTimeoutResponseTest_data();
	void CreateHttpRequestTimeoutResponseTest();
	void CreateHttpRequestTimeoutResponseIgnoresTimeZoneTest();

	// 400 response of HTTP
	void CreateHttpBadRequestResponseTest();
	void HttpBadRequestMessageIsCreatedAtCallTest();

	// 503 response of HTTP
	void CreateHttpProcessingTimeoutResponseTest();
	void HttpProcessingTimeoutMessageIsCreatedAtCallTest();

	// 500 response of HTTP
	void CreateHttpInternalServerErrorResponseTest();
	void CreateHttpInternalServerErrorResponseClosingTest();
	void HttpServerErrorMessageIsCreatedAtCallTest();

	// Behavior of the protocols
	void ProtocolTypeIdsTest();
	void DefaultBehaviorTest();
	void HttpBehaviorTest_data();
	void HttpBehaviorTest();
	void HttpBehaviorCreatesCurrentDateTest();
	void RequestResponseCycleTest();
	void OtherProtocolBehaviorTest_data();
	void OtherProtocolBehaviorTest();
};


