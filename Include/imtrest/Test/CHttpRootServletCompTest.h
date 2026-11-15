// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>


class CHttpRootServletCompTest: public QObject
{
	Q_OBJECT
private slots:
	void DeflateRoundTripTest();
	void GzipHeaderTest();
	void SameTagIsCompressedOnceTest();
	void TagHeaderNameIgnoresCaseTest();
	void NewTagIsCompressedAgainTest();
	void OtherCommandIsCompressedApartTest();
	void EncodingsAreCachedApartTest();
	void ResponseWithoutTagIsAlwaysCompressedTest();
};


