// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>


class CHttpRequestBodyPolicyTest: public QObject
{
	Q_OBJECT
private slots:
	// Without a policy and a limit the requests are received as before
	void NoPolicyNoLimitTest();
	void HugeContentLengthDoesNotReserveMemoryTest();

	// Limit of the body
	void DefaultLimitRefusesBigContentLengthTest();
	void ContentLengthEqualToLimitTest();
	void BodyAfterRefusalIsNotReceivedTest();
	void ChunkedBodyOverLimitTest();
	void ChunkedBodyWithinLimitTest();
	void LimitOfPolicyTest_data();
	void LimitOfPolicyTest();

	// Refusal by the policy
	void PolicyRefusesRequestTest();
	void PolicyRefusalWithoutStatusTest();
	void PolicySeesHeadOfRequestTest();
	void PolicyIsCalledOnceTest();
	void PolicyIsNotCalledBeforeEndOfHeadersTest();
	void PolicyDecidesAlsoForRequestWithoutBodyTest();
	void ResetBodyPolicyTest();
	void ResetBodyPolicyKeepsDefaultLimitTest();

	// Message of the refusal
	void RefusalMessageIsFinalResponseTest();
	void RefusalWithUnknownStatusTest();
	void RefusalWhenEngineCreatesNoResponseTest();
	void RefusalIsKeptTest();
	void ResetDataClearsRefusalTest();

	// Expect: 100-continue
	void ExpectContinueGetsInterimMessageTest();
	void InterimMessageIsTakenOnceTest();
	void ExpectContinueValueTest_data();
	void ExpectContinueValueTest();
	void ExpectContinueWithChunkedBodyTest();
	void ExpectWithoutBodyIsIgnoredTest_data();
	void ExpectWithoutBodyIsIgnoredTest();
	void ExpectIsIgnoredForHttp10Test();
	void ExpectWithReceivedBodyIsIgnoredTest();
	void BodyAfterInterimMessageIsReceivedTest();
	void UnknownExpectationIsRefusedTest_data();
	void UnknownExpectationIsRefusedTest();
	void ExpectIsRefusedByPolicyTest();
	void ExpectIsRefusedByLimitTest();
	void PolicyRefusalBeatsUnknownExpectationTest();
	void ExpectAcceptedByPolicyTest();
	void NoExpectNoActionTest();
};


