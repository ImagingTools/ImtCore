// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CPaintConvertersTest.h"

// Qt includes
#include <QtGui/QBrush>
#include <QtGui/QColor>
#include <QtGui/QLinearGradient>
#include <QtGui/QPen>
#include <QtGui/QTransform>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imt2d/imt2d.h>


void CPaintConvertersTest::testPenRoundTrip()
{
	QPen input;
	input.setColor(QColor(10, 20, 30, 140));
	input.setWidthF(2.5);

	const QVector<qreal> dashPattern = {1.0, 2.0, 3.0};
	input.setStyle(Qt::CustomDashLine);
	input.setDashPattern(dashPattern);
	input.setDashOffset(0.5);
	input.setCapStyle(Qt::RoundCap);
	input.setJoinStyle(Qt::BevelJoin);
	input.setMiterLimit(4.0);
	input.setCosmetic(true);

	sdl::V1_0::imt2d::CPen sdlPen;
	imt2d::CPaintConverters::ToSdlPen(input, sdlPen);

	QPen output;
	QVERIFY(imt2d::CPaintConverters::ToQtPen(sdlPen, output));

	QCOMPARE(output.style(), input.style());
	QCOMPARE(output.capStyle(), input.capStyle());
	QCOMPARE(output.joinStyle(), input.joinStyle());
	QCOMPARE(output.color().alpha(), input.color().alpha());
	QCOMPARE(output.color().name(QColor::HexRgb), input.color().name(QColor::HexRgb));
	QCOMPARE(output.isCosmetic(), input.isCosmetic());
	QVERIFY(qAbs(output.widthF() - input.widthF()) < 0.001);
}


void CPaintConvertersTest::testBrushRoundTripWithoutGradient()
{
	QBrush input(QColor(100, 120, 140, 160), Qt::DiagCrossPattern);
	input.setTransform(QTransform(1.0, 0.1, 0.0, 0.2, 1.0, 0.0, 5.0, 7.0, 1.0));

	sdl::V1_0::imt2d::CBrush sdlBrush;
	imt2d::CPaintConverters::ToSdlBrush(input, sdlBrush);

	QBrush output;
	QVERIFY(imt2d::CPaintConverters::ToQtBrush(sdlBrush, output));

	QCOMPARE(output.style(), input.style());
	QCOMPARE(output.color().alpha(), input.color().alpha());
	QCOMPARE(output.color().name(QColor::HexRgb), input.color().name(QColor::HexRgb));
	QCOMPARE(output.transform(), input.transform());
}


void CPaintConvertersTest::testBrushGradientStyleRoundTrip()
{
	QLinearGradient gradient(0.0, 0.0, 10.0, 10.0);
	gradient.setColorAt(0.0, QColor(255, 0, 0, 255));
	gradient.setColorAt(1.0, QColor(0, 0, 255, 255));
	QBrush input(gradient);

	sdl::V1_0::imt2d::CBrush sdlBrush;
	imt2d::CPaintConverters::ToSdlBrush(input, sdlBrush);

	QBrush output;
	QVERIFY(imt2d::CPaintConverters::ToQtBrush(sdlBrush, output));
	QCOMPARE(output.style(), Qt::LinearGradientPattern);
}


void CPaintConvertersTest::testTransformRoundTrip()
{
	QTransform input(1.0, 2.0, 3.0,
		4.0, 5.0, 6.0,
		7.0, 8.0, 9.0);

	sdl::V1_0::imt2d::CTransform2d sdlTransform;
	imt2d::CPaintConverters::ToSdlTransform(input, sdlTransform);
	const QTransform output = imt2d::CPaintConverters::ToQtTransform(sdlTransform);

	QCOMPARE(output, input);
}


I_ADD_TEST(CPaintConvertersTest);
