// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>


class CWorkerTest: public QObject
{
	Q_OBJECT
private slots:
	// A servlet that works correctly
	void ResponseOfServletIsReturnedTest();
	void ServletGetsRequestAndSubCommandTest();
	void ServletIsCalledOnceTest();

	// HTTP: each request has exactly one response
	void ExceptionGetsInternalErrorTest();
	void UnknownExceptionGetsInternalErrorTest();
	void NoResponseGetsInternalErrorTest();
	void MissingServletGetsInternalErrorTest();
	void InternalErrorResponseHasNoContentTest();
	void InternalErrorResponseUsesEngineOfRequestTest();
	void FailureDoesNotAffectNextRequestTest();
	void EngineWithoutResponseTest();

	// Other protocols: a message can have no response
	void FailureOfOtherProtocolHasNoResponseTest_data();
	void FailureOfOtherProtocolHasNoResponseTest();
	void ResponseOfOtherProtocolIsReturnedTest();
	void CreateInternalErrorResponseOfOtherProtocolTest();
};


