// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>


class CHttpSenderTest: public QObject
{
	Q_OBJECT
private slots:
	// Status line, body and framing
	void ExactResponseTest();
	void EmptyBodyTest();
	void BinaryBodyTest();
	void UnknownStatusTest();
	void NoContentTypeWithoutDataTypeTest();

	// Content-Length
	void OwnContentLengthIsReplacedTest_data();
	void OwnContentLengthIsReplacedTest();
	void TransferEncodingHasNoContentLengthTest();
	void NoContentStatusesTest_data();
	void NoContentStatusesTest();
	void NotModifiedKeepsOwnContentLengthTest_data();
	void NotModifiedKeepsOwnContentLengthTest();
	void ResetContentTest();
	void HeadResponseTest();
	void HeadResponseWithOwnLengthTest_data();
	void HeadResponseWithOwnLengthTest();

	// Content-Type and other headers of the response
	void OwnContentTypeIsNotDuplicatedTest_data();
	void OwnContentTypeIsNotDuplicatedTest();
	void OtherHeadersAreKeptTest();

	// Date
	void DateIsGeneratedTest_data();
	void DateIsGeneratedTest();
	void OwnDateIsKeptTest();
	void DateIsTakenFromClockTest();

	// Connection
	void ConnectionCloseTest();
	void ConnectionOfResponseIsControlledByServerTest_data();
	void ConnectionOfResponseIsControlledByServerTest();

	// Security
	void InvalidHeadersAreSkippedTest_data();
	void InvalidHeadersAreSkippedTest();
	void InvalidDataTypeIsSkippedTest();

	// Conformance of the produced messages
	void ResponsesAreParsedByHttpParserTest();
	void ResponsesWithConnectionCloseAreParsedTest();
};


