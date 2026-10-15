// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>


class CHttpRequestTest: public QObject
{
	Q_OBJECT
private slots:
	void ParseWholeRequestTest();
	void ParseByteByByteTest();
	void ParseSplitAtEveryPositionTest();
	void SplitHeaderNameTest();
	void SplitHeaderValueTest();
	void SplitUrlAndHostTest();
	void RepeatedHeadersTest();
	void EmptyHeaderValueTest();
	void HeaderNamesAreLowerCaseTest();
	void HeaderValueWithColonsTest();
	void MissingHeaderTest();

	void MethodTypeTest_data();
	void MethodTypeTest();
	void StateTransitionsTest();
	void SchemeWithoutSocketTest();

	void CommandParamsTest();
	void CommandParamsWithoutQueryTest();

	void ChunkedBodyTest();
	void ChunkedBodySplitAtEveryPositionTest();
	void BinaryBodyTest();
	void LargeBodyTest();
	void LongHeaderValueTest();
	void HeadersAboveLimitTest();
	void InvalidRequestTest();

	void ResetDataTest();
	void SecondRequestOnSameObjectTest();
	void RequestIdTest();
	void SettersTest();
};


