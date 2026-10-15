// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>


class CHttpRangeToolsTest: public QObject
{
	Q_OBJECT
private slots:
	// Content-Range: parsing
	void ParseContentRangeTest_data();
	void ParseContentRangeTest();
	void ParseContentRangeInvalidTest_data();
	void ParseContentRangeInvalidTest();
	void ParseContentRangeKeepsResultOnErrorTest();

	// Content-Range: description of the range
	void ContentRangeLengthTest();

	// Content-Range: creation
	void CreateContentRangeTest_data();
	void CreateContentRangeTest();
	void CreateContentRangeRoundTripTest();
};


