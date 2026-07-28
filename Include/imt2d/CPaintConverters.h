// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtGui/QBrush>
#include <QtGui/QColor>
#include <QtGui/QPen>
#include <QtGui/QTransform>

// Generated includes
#include <GeneratedFiles/imt2dsdl/SDL/1.0/CPP/PaintPrimitives.h>


namespace imt2d
{

class CPaintConverters
{
public:
	CPaintConverters() = delete;

	static QColor ToQColor(const sdl::V1_0::imtcolor::CRgbColorHex& sdlColor, int alpha = 255);
	static sdl::V1_0::imtcolor::CRgbColorHex ToSdlColor(const QColor& color);

	static Qt::PenStyle ToQtPenStyle(sdl::V1_0::imt2d::PenStyle style);
	static sdl::V1_0::imt2d::PenStyle ToSdlPenStyle(Qt::PenStyle style);

	static Qt::PenCapStyle ToQtPenCapStyle(sdl::V1_0::imt2d::PenCapStyle style);
	static sdl::V1_0::imt2d::PenCapStyle ToSdlPenCapStyle(Qt::PenCapStyle style);

	static Qt::PenJoinStyle ToQtPenJoinStyle(sdl::V1_0::imt2d::PenJoinStyle style);
	static sdl::V1_0::imt2d::PenJoinStyle ToSdlPenJoinStyle(Qt::PenJoinStyle style);

	static Qt::BrushStyle ToQtBrushStyle(sdl::V1_0::imt2d::BrushStyle style);
	static sdl::V1_0::imt2d::BrushStyle ToSdlBrushStyle(Qt::BrushStyle style);

	static QGradient::Spread ToQtGradientSpread(sdl::V1_0::imt2d::GradientSpread spread);
	static sdl::V1_0::imt2d::GradientSpread ToSdlGradientSpread(QGradient::Spread spread);

	static QGradient::CoordinateMode ToQtGradientCoordinateMode(sdl::V1_0::imt2d::GradientCoordinateMode mode);
	static sdl::V1_0::imt2d::GradientCoordinateMode ToSdlGradientCoordinateMode(QGradient::CoordinateMode mode);

	static QTransform ToQtTransform(const sdl::V1_0::imt2d::CTransform2d& sdlTransform);
	static void ToSdlTransform(const QTransform& transform, sdl::V1_0::imt2d::CTransform2d& sdlTransform);

	static bool ToQtBrush(const sdl::V1_0::imt2d::CBrush& sdlBrush, QBrush& brush);
	static void ToSdlBrush(const QBrush& brush, sdl::V1_0::imt2d::CBrush& sdlBrush);

	static bool ToQtPen(const sdl::V1_0::imt2d::CPen& sdlPen, QPen& pen);
	static void ToSdlPen(const QPen& pen, sdl::V1_0::imt2d::CPen& sdlPen);
};

} // namespace imt2d
