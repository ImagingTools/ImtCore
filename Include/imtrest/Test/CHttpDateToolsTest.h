// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>


class CHttpDateToolsTest: public QObject
{
	Q_OBJECT
private slots:
	void ToImfFixdateTest_data();
	void ToImfFixdateTest();
	void TimeZoneIsIgnoredTest();
	void MonthNamesTest();
	void DayNamesTest();
	void NamesAreNotLocalizedTest();
	void FixedLengthTest();

	// Parsing
	void FromHttpDateTest_data();
	void FromHttpDateTest();
	void FromHttpDateIsUtcTest();
	void RoundTripTest();
	void Rfc850YearTest_data();
	void Rfc850YearTest();
	void Rfc850YearInNextCenturyTest();
	void LeapSecondTest();
	void SpacesAreIgnoredTest();
	void DayNameIsNotComparedWithDateTest();
	void InvalidDateTest_data();
	void InvalidDateTest();
	void TruncatedValuesAreInvalidTest_data();
	void TruncatedValuesAreInvalidTest();
	void CorruptedValuesTest();
};


