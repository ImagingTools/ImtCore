// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imt3d/imt3d.h>


/**
	Unit test for imt3d conversion functions.
*/
class CConversionTest: public QObject
{
	Q_OBJECT

private slots:
	void initTestCase();

	void testConvertDepthImageToCloud();
	void testConvertPointCloudToDepthBitmap();

	void cleanupTestCase();
};


