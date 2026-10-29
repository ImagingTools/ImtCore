// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>


/**
	Tests for the C++ header a product is exported to (\c <Product>Features.h) and imported from.
*/
class CProductFeaturesFileTest: public QObject
{
	Q_OBJECT

private Q_SLOTS:
	// What is written is read back
	void testProductSurvivesRoundTrip();
	void testFeatureTreeSurvivesRoundTrip();
	void testQuotesAndBackslashesSurviveRoundTrip();
	void testReadProductIsNew();

	// What is not a product header
	void testUnknownStatementIsRefused();
	void testFileWithoutProductIdIsRefused();
	void testFeatureNeverAddedIsRefused();
	void testUnknownFeatureVariableIsRefused();
	void testRootFeatureAddedUnderAnotherUuidIsRefused();
};


