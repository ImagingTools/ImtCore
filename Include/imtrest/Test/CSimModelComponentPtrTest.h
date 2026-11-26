// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>


/**
	Tests of TSimModelComponentPtr, the simulated model of the tests: with MSVC a simulated model with an attribute or a reference fails
	in its destructor if it still has its context (ACF).
*/
class CSimModelComponentPtrTest: public QObject
{
	Q_OBJECT
private slots:
	void ModelWithAttributesIsDestroyedTest();
};


