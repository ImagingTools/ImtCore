// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QObject>


class CPaintConvertersTest: public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void testPenRoundTrip();
	void testBrushRoundTripWithoutGradient();
	void testBrushGradientStyleRoundTrip();
	void testTransformRoundTrip();
};
