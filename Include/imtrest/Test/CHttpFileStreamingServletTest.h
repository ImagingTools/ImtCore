// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>


class CHttpFileStreamingServletTest: public QObject
{
	Q_OBJECT
private slots:
	// Which part of the file is sent
	void WholeFileTest();
	void HeadTest();
	void RangeTest_data();
	void RangeTest();
	void RangeIsIgnoredTest_data();
	void RangeIsIgnoredTest();
	void OverlappingRangesTest();
	void RangeNotSatisfiableTest_data();
	void RangeNotSatisfiableTest();
	void EmptyFileTest();

	// Conditions
	void IfRangeTest_data();
	void IfRangeTest();
	void PreconditionsTest_data();
	void PreconditionsTest();
	void PreconditionIsBeforeRangeTest();

	// Headers and the body
	void HeadersOfResponsesTest();
	void ContentTypeTest();
	void HeadOfResponseTest();
	void BodySourceOfChangedFileTest();
	void DefaultETagTest();

	// The servlet
	void ServletGetTest();
	void ServletHeadTest();
	void ServletHeadersReachDecisionsTest();
	void ServletDefaultETagTest();
	void ServletOwnETagTest();
	void ServletRefusalTest_data();
	void ServletRefusalTest();
	void ServletMissingFileTest();
	void ServletRefusalKeepsHeadersTest();
	void ServletWithoutResolverTest();
};


