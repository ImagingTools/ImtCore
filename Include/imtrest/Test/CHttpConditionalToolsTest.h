// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>


class CHttpConditionalToolsTest: public QObject
{
	Q_OBJECT
private slots:
	// Entity tags
	void ParseEntityTagTest_data();
	void ParseEntityTagTest();
	void ParseEntityTagInvalidTest_data();
	void ParseEntityTagInvalidTest();
	void ParseEntityTagKeepsResultOnErrorTest();
	void CreateEntityTagTest_data();
	void CreateEntityTagTest();
	void CreateEntityTagInvalidTest_data();
	void CreateEntityTagInvalidTest();
	void CreateEntityTagRoundTripTest();

	// Comparison with the list of entity tags
	void MatchEntityTagListTest_data();
	void MatchEntityTagListTest();
	void MatchEntityTagListInvalidTest_data();
	void MatchEntityTagListInvalidTest();

	// If-Match, If-None-Match, If-Modified-Since, If-Unmodified-Since
	void EvaluatePreconditionsTest_data();
	void EvaluatePreconditionsTest();
	void EvaluatePreconditionsFractionsOfSecondTest();

	// If-Range
	void EvaluateIfRangeTest_data();
	void EvaluateIfRangeTest();
	void EvaluateIfRangeFractionsOfSecondTest();
};


