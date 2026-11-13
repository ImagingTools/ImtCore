// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>


class CHttpRequestBodyStreamTest: public QObject
{
	Q_OBJECT
private slots:
	// The body goes to the receiver
	void StreamedBodyGoesToSinkTest();
	void BodyInPartsTest();
	void ChunkedBodyTest();
	void EmptyBodyFinishesSinkTest_data();
	void EmptyBodyFinishesSinkTest();
	void BigBodyIsNotBufferedTest();
	void BufferModeIgnoresSinkTest();
	void GetBodySinkTest();
	void NextRequestOfObjectTest();

	// Limit of the body
	void BigContentLengthIsRefusedBeforeBodyTest();
	void ChunkedBodyOverLimitTest();
	void LimitCountsAllPartsTest();
	void BodyWithinLimitTest();

	// Failures
	void WriteFailureGetsInternalErrorTest();
	void FinishFailureGetsInternalErrorTest();
	void MissingSinkGetsInternalErrorTest();

	// The receiver is released
	void UnfinishedSinkIsAbortedOnDestructionTest();
	void FinishedSinkIsNotAbortedTest();
	void RefusedRequestAbortsSinkOnceTest();
	void UnknownExpectationAbortsSinkTest();
	void ResetDataAbortsUnfinishedSinkTest();
	void ResetDataOfFinishedRequestDoesNotAbortTest();

	// Expect: 100-continue
	void ExpectContinueWithStreamTest();
};


