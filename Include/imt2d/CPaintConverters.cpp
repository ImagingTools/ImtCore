// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial

#include <imt2d/CPaintConverters.h>

// STL includes
#include <variant>

// Qt includes
#include <QtCore/QBuffer>
#include <QtCore/QByteArray>
#include <QtCore/QIODevice>
#include <QtCore/QString>
#include <QtGui/QImage>
#include <QtGui/QLinearGradient>
#include <QtGui/QRadialGradient>
#include <QtGui/QConicalGradient>



namespace
{

QByteArray GetFormatHint(sdl::V1_0::imt2d::ImageFormat format)
{
	switch (format){
	case sdl::V1_0::imt2d::ImageFormat::JPEG:
		return "JPEG";
	case sdl::V1_0::imt2d::ImageFormat::SVG:
		return "SVG";
	default:
		return "PNG";
	}
}


double GetPointValue(const istd::TNullableValue<double>& value, double defaultValue)
{
	return value.value_or(defaultValue);
}


QPointF ToQPointF(const sdl::V1_0::imt2d::CPoint2d& point)
{
	return {GetPointValue(point.x, 0.0), GetPointValue(point.y, 0.0)};
}


void ToSdlPoint(const QPointF& point, sdl::V1_0::imt2d::CPoint2d& sdlPoint)
{
	sdlPoint.x = point.x();
	sdlPoint.y = point.y();
}


void ApplyStopsToQtGradient(const imtsdl::TElementList<sdl::V1_0::imt2d::CGradientStop>& sdlStops, QGradient& gradient)
{
	for (const auto& sdlStopPtr : sdlStops) {
		if (!sdlStopPtr || !sdlStopPtr->position || !sdlStopPtr->color) {
			continue;
		}

		const int alpha = sdlStopPtr->alpha.value_or(255);
		gradient.setColorAt(*sdlStopPtr->position, imt2d::CPaintConverters::ToQColor(*sdlStopPtr->color, alpha));
	}
}


void FillSdlStopsFromQtGradient(const QGradient& gradient, imtsdl::TElementList<sdl::V1_0::imt2d::CGradientStop>& sdlStops)
{
	const QGradientStops stops = gradient.stops();
	for (const QGradientStop& stop : stops) {
		sdl::V1_0::imt2d::CGradientStop sdlStop;
		sdlStop.position = stop.first;
		sdlStop.color.emplace() = imt2d::CPaintConverters::ToSdlColor(stop.second);
		sdlStop.alpha = stop.second.alpha();
		sdlStops.append(sdlStop);
	}
}


void ApplyLinearGradientToBrush(const sdl::V1_0::imt2d::CLinearGradient& linear, QBrush& brush)
{
	QLinearGradient gradient(
		ToQPointF(linear.start.value_or(sdl::V1_0::imt2d::CPoint2d())),
		ToQPointF(linear.end.value_or(sdl::V1_0::imt2d::CPoint2d())));

	if (linear.spread) {
		gradient.setSpread(imt2d::CPaintConverters::ToQtGradientSpread(*linear.spread));
	}
	if (linear.coordinateMode) {
		gradient.setCoordinateMode(imt2d::CPaintConverters::ToQtGradientCoordinateMode(*linear.coordinateMode));
	}
	if (linear.stops) {
		ApplyStopsToQtGradient(*linear.stops, gradient);
	}

	brush = QBrush(gradient);
}


void ApplyRadialGradientToBrush(const sdl::V1_0::imt2d::CRadialGradient& radial, QBrush& brush)
{
	const sdl::V1_0::imt2d::CPoint2d center = radial.center.value_or(sdl::V1_0::imt2d::CPoint2d());
	QRadialGradient gradient(
		ToQPointF(center),
		radial.radius.value_or(0.0),
		ToQPointF(radial.focalPoint.value_or(center)));

	if (radial.spread) {
		gradient.setSpread(imt2d::CPaintConverters::ToQtGradientSpread(*radial.spread));
	}
	if (radial.coordinateMode) {
		gradient.setCoordinateMode(imt2d::CPaintConverters::ToQtGradientCoordinateMode(*radial.coordinateMode));
	}
	if (radial.stops) {
		ApplyStopsToQtGradient(*radial.stops, gradient);
	}

	brush = QBrush(gradient);
}


void ApplyConicalGradientToBrush(const sdl::V1_0::imt2d::CConicalGradient& conical, QBrush& brush)
{
	QConicalGradient gradient(
		ToQPointF(conical.center.value_or(sdl::V1_0::imt2d::CPoint2d())),
		conical.angle.value_or(0.0));

	if (conical.coordinateMode) {
		gradient.setCoordinateMode(imt2d::CPaintConverters::ToQtGradientCoordinateMode(*conical.coordinateMode));
	}
	if (conical.stops) {
		ApplyStopsToQtGradient(*conical.stops, gradient);
	}

	brush = QBrush(gradient);
}


void ApplyGradientToBrush(const sdl::V1_0::imt2d::CBrush& sdlBrush, QBrush& brush)
{
	if (!sdlBrush.gradient) {
		return;
	}

	if (const auto* linear = std::get_if<sdl::V1_0::imt2d::CLinearGradient>(sdlBrush.gradient.GetPtr())) {
		ApplyLinearGradientToBrush(*linear, brush);
		return;
	}

	if (const auto* radial = std::get_if<sdl::V1_0::imt2d::CRadialGradient>(sdlBrush.gradient.GetPtr())) {
		ApplyRadialGradientToBrush(*radial, brush);
		return;
	}

	if (const auto* conical = std::get_if<sdl::V1_0::imt2d::CConicalGradient>(sdlBrush.gradient.GetPtr())) {
		ApplyConicalGradientToBrush(*conical, brush);
	}
}


void ApplyTextureToBrush(const sdl::V1_0::imt2d::CBrush& sdlBrush, QBrush& brush)
{
	if (!sdlBrush.texture) {
		return;
	}

	const auto& sdlTexture = *sdlBrush.texture;
	if (!sdlTexture.imageData || !sdlTexture.imageFormat) {
		return;
	}

	const QByteArray raw = QByteArray::fromBase64((*sdlTexture.imageData).toLatin1());
	if (raw.isEmpty()) {
		return;
	}

	QImage image;
	const QByteArray formatHint = GetFormatHint(*sdlTexture.imageFormat);
	image.loadFromData(raw, formatHint.constData());
	if (!image.isNull()) {
		brush.setTextureImage(image);
	}
}


} // namespace


namespace imt2d
{

QColor CPaintConverters::ToQColor(const sdl::V1_0::imtcolor::CRgbColorHex& sdlColor, int alpha)
{
	QColor result(sdlColor.Color.value_or(QString()));
	if (!result.isValid()) {
		result = QColor(Qt::black);
	}
	result.setAlpha(alpha);

	return result;
}


sdl::V1_0::imtcolor::CRgbColorHex CPaintConverters::ToSdlColor(const QColor& color)
{
	sdl::V1_0::imtcolor::CRgbColorHex result;
	result.Color = color.name(QColor::HexRgb);
	return result;
}


Qt::PenStyle CPaintConverters::ToQtPenStyle(sdl::V1_0::imt2d::PenStyle style)
{
	switch (style){
	case sdl::V1_0::imt2d::PenStyle::NO_PEN:
		return Qt::NoPen;
	case sdl::V1_0::imt2d::PenStyle::DASH_LINE:
		return Qt::DashLine;
	case sdl::V1_0::imt2d::PenStyle::DOT_LINE:
		return Qt::DotLine;
	case sdl::V1_0::imt2d::PenStyle::DASH_DOT_LINE:
		return Qt::DashDotLine;
	case sdl::V1_0::imt2d::PenStyle::DASH_DOT_DOT_LINE:
		return Qt::DashDotDotLine;
	case sdl::V1_0::imt2d::PenStyle::CUSTOM_DASH_LINE:
		return Qt::CustomDashLine;
	default:
		return Qt::SolidLine;
	}
}


sdl::V1_0::imt2d::PenStyle CPaintConverters::ToSdlPenStyle(Qt::PenStyle style)
{
	switch (style){
	case Qt::NoPen:
		return sdl::V1_0::imt2d::PenStyle::NO_PEN;
	case Qt::DashLine:
		return sdl::V1_0::imt2d::PenStyle::DASH_LINE;
	case Qt::DotLine:
		return sdl::V1_0::imt2d::PenStyle::DOT_LINE;
	case Qt::DashDotLine:
		return sdl::V1_0::imt2d::PenStyle::DASH_DOT_LINE;
	case Qt::DashDotDotLine:
		return sdl::V1_0::imt2d::PenStyle::DASH_DOT_DOT_LINE;
	case Qt::CustomDashLine:
		return sdl::V1_0::imt2d::PenStyle::CUSTOM_DASH_LINE;
	default:
		return sdl::V1_0::imt2d::PenStyle::SOLID_LINE;
	}
}


Qt::PenCapStyle CPaintConverters::ToQtPenCapStyle(sdl::V1_0::imt2d::PenCapStyle style)
{
	switch (style){
	case sdl::V1_0::imt2d::PenCapStyle::FLAT_CAP:
		return Qt::FlatCap;
	case sdl::V1_0::imt2d::PenCapStyle::ROUND_CAP:
		return Qt::RoundCap;
	default:
		return Qt::SquareCap;
	}
}


sdl::V1_0::imt2d::PenCapStyle CPaintConverters::ToSdlPenCapStyle(Qt::PenCapStyle style)
{
	switch (style){
	case Qt::FlatCap:
		return sdl::V1_0::imt2d::PenCapStyle::FLAT_CAP;
	case Qt::RoundCap:
		return sdl::V1_0::imt2d::PenCapStyle::ROUND_CAP;
	default:
		return sdl::V1_0::imt2d::PenCapStyle::SQUARE_CAP;
	}
}


Qt::PenJoinStyle CPaintConverters::ToQtPenJoinStyle(sdl::V1_0::imt2d::PenJoinStyle style)
{
	switch (style){
	case sdl::V1_0::imt2d::PenJoinStyle::BEVEL_JOIN:
		return Qt::BevelJoin;
	case sdl::V1_0::imt2d::PenJoinStyle::ROUND_JOIN:
		return Qt::RoundJoin;
	case sdl::V1_0::imt2d::PenJoinStyle::SVG_MITER_JOIN:
		return Qt::SvgMiterJoin;
	default:
		return Qt::MiterJoin;
	}
}


sdl::V1_0::imt2d::PenJoinStyle CPaintConverters::ToSdlPenJoinStyle(Qt::PenJoinStyle style)
{
	switch (style){
	case Qt::BevelJoin:
		return sdl::V1_0::imt2d::PenJoinStyle::BEVEL_JOIN;
	case Qt::RoundJoin:
		return sdl::V1_0::imt2d::PenJoinStyle::ROUND_JOIN;
	case Qt::SvgMiterJoin:
		return sdl::V1_0::imt2d::PenJoinStyle::SVG_MITER_JOIN;
	default:
		return sdl::V1_0::imt2d::PenJoinStyle::MITER_JOIN;
	}
}


Qt::BrushStyle CPaintConverters::ToQtBrushStyle(sdl::V1_0::imt2d::BrushStyle style)
{
	switch (style){
	case sdl::V1_0::imt2d::BrushStyle::NO_BRUSH:
		return Qt::NoBrush;
	case sdl::V1_0::imt2d::BrushStyle::DENSE1_PATTERN:
		return Qt::Dense1Pattern;
	case sdl::V1_0::imt2d::BrushStyle::DENSE2_PATTERN:
		return Qt::Dense2Pattern;
	case sdl::V1_0::imt2d::BrushStyle::DENSE3_PATTERN:
		return Qt::Dense3Pattern;
	case sdl::V1_0::imt2d::BrushStyle::DENSE4_PATTERN:
		return Qt::Dense4Pattern;
	case sdl::V1_0::imt2d::BrushStyle::DENSE5_PATTERN:
		return Qt::Dense5Pattern;
	case sdl::V1_0::imt2d::BrushStyle::DENSE6_PATTERN:
		return Qt::Dense6Pattern;
	case sdl::V1_0::imt2d::BrushStyle::DENSE7_PATTERN:
		return Qt::Dense7Pattern;
	case sdl::V1_0::imt2d::BrushStyle::HOR_PATTERN:
		return Qt::HorPattern;
	case sdl::V1_0::imt2d::BrushStyle::VER_PATTERN:
		return Qt::VerPattern;
	case sdl::V1_0::imt2d::BrushStyle::CROSS_PATTERN:
		return Qt::CrossPattern;
	case sdl::V1_0::imt2d::BrushStyle::BDIAG_PATTERN:
		return Qt::BDiagPattern;
	case sdl::V1_0::imt2d::BrushStyle::FDIAG_PATTERN:
		return Qt::FDiagPattern;
	case sdl::V1_0::imt2d::BrushStyle::DIAG_CROSS_PATTERN:
		return Qt::DiagCrossPattern;
	case sdl::V1_0::imt2d::BrushStyle::LINEAR_GRADIENT_PATTERN:
		return Qt::LinearGradientPattern;
	case sdl::V1_0::imt2d::BrushStyle::RADIAL_GRADIENT_PATTERN:
		return Qt::RadialGradientPattern;
	case sdl::V1_0::imt2d::BrushStyle::CONICAL_GRADIENT_PATTERN:
		return Qt::ConicalGradientPattern;
	case sdl::V1_0::imt2d::BrushStyle::TEXTURE_PATTERN:
		return Qt::TexturePattern;
	default:
		return Qt::SolidPattern;
	}
}


sdl::V1_0::imt2d::BrushStyle CPaintConverters::ToSdlBrushStyle(Qt::BrushStyle style)
{
	switch (style){
	case Qt::NoBrush:
		return sdl::V1_0::imt2d::BrushStyle::NO_BRUSH;
	case Qt::Dense1Pattern:
		return sdl::V1_0::imt2d::BrushStyle::DENSE1_PATTERN;
	case Qt::Dense2Pattern:
		return sdl::V1_0::imt2d::BrushStyle::DENSE2_PATTERN;
	case Qt::Dense3Pattern:
		return sdl::V1_0::imt2d::BrushStyle::DENSE3_PATTERN;
	case Qt::Dense4Pattern:
		return sdl::V1_0::imt2d::BrushStyle::DENSE4_PATTERN;
	case Qt::Dense5Pattern:
		return sdl::V1_0::imt2d::BrushStyle::DENSE5_PATTERN;
	case Qt::Dense6Pattern:
		return sdl::V1_0::imt2d::BrushStyle::DENSE6_PATTERN;
	case Qt::Dense7Pattern:
		return sdl::V1_0::imt2d::BrushStyle::DENSE7_PATTERN;
	case Qt::HorPattern:
		return sdl::V1_0::imt2d::BrushStyle::HOR_PATTERN;
	case Qt::VerPattern:
		return sdl::V1_0::imt2d::BrushStyle::VER_PATTERN;
	case Qt::CrossPattern:
		return sdl::V1_0::imt2d::BrushStyle::CROSS_PATTERN;
	case Qt::BDiagPattern:
		return sdl::V1_0::imt2d::BrushStyle::BDIAG_PATTERN;
	case Qt::FDiagPattern:
		return sdl::V1_0::imt2d::BrushStyle::FDIAG_PATTERN;
	case Qt::DiagCrossPattern:
		return sdl::V1_0::imt2d::BrushStyle::DIAG_CROSS_PATTERN;
	case Qt::LinearGradientPattern:
		return sdl::V1_0::imt2d::BrushStyle::LINEAR_GRADIENT_PATTERN;
	case Qt::RadialGradientPattern:
		return sdl::V1_0::imt2d::BrushStyle::RADIAL_GRADIENT_PATTERN;
	case Qt::ConicalGradientPattern:
		return sdl::V1_0::imt2d::BrushStyle::CONICAL_GRADIENT_PATTERN;
	case Qt::TexturePattern:
		return sdl::V1_0::imt2d::BrushStyle::TEXTURE_PATTERN;
	default:
		return sdl::V1_0::imt2d::BrushStyle::SOLID_PATTERN;
	}
}


QGradient::Spread CPaintConverters::ToQtGradientSpread(sdl::V1_0::imt2d::GradientSpread spread)
{
	switch (spread){
	case sdl::V1_0::imt2d::GradientSpread::REFLECT_SPREAD:
		return QGradient::ReflectSpread;
	case sdl::V1_0::imt2d::GradientSpread::REPEAT_SPREAD:
		return QGradient::RepeatSpread;
	default:
		return QGradient::PadSpread;
	}
}


sdl::V1_0::imt2d::GradientSpread CPaintConverters::ToSdlGradientSpread(QGradient::Spread spread)
{
	switch (spread){
	case QGradient::ReflectSpread:
		return sdl::V1_0::imt2d::GradientSpread::REFLECT_SPREAD;
	case QGradient::RepeatSpread:
		return sdl::V1_0::imt2d::GradientSpread::REPEAT_SPREAD;
	default:
		return sdl::V1_0::imt2d::GradientSpread::PAD_SPREAD;
	}
}


QGradient::CoordinateMode CPaintConverters::ToQtGradientCoordinateMode(sdl::V1_0::imt2d::GradientCoordinateMode mode)
{
	switch (mode){
	case sdl::V1_0::imt2d::GradientCoordinateMode::STRETCH_TO_DEVICE_MODE:
		return QGradient::StretchToDeviceMode;
	case sdl::V1_0::imt2d::GradientCoordinateMode::OBJECT_BOUNDING_MODE:
		return QGradient::ObjectBoundingMode;
	case sdl::V1_0::imt2d::GradientCoordinateMode::OBJECT_MODE:
		return QGradient::ObjectMode;
	default:
		return QGradient::LogicalMode;
	}
}


sdl::V1_0::imt2d::GradientCoordinateMode CPaintConverters::ToSdlGradientCoordinateMode(QGradient::CoordinateMode mode)
{
	switch (mode){
	case QGradient::StretchToDeviceMode:
		return sdl::V1_0::imt2d::GradientCoordinateMode::STRETCH_TO_DEVICE_MODE;
	case QGradient::ObjectBoundingMode:
		return sdl::V1_0::imt2d::GradientCoordinateMode::OBJECT_BOUNDING_MODE;
	case QGradient::ObjectMode:
		return sdl::V1_0::imt2d::GradientCoordinateMode::OBJECT_MODE;
	default:
		return sdl::V1_0::imt2d::GradientCoordinateMode::LOGICAL_MODE;
	}
}


QTransform CPaintConverters::ToQtTransform(const sdl::V1_0::imt2d::CTransform2d& sdlTransform)
{
	auto getOrDefault = [](const istd::TNullableValue<double>& value, double fallback) {
		return value.value_or(fallback);
	};

	return {
		getOrDefault(sdlTransform.m11, 1.0),
		getOrDefault(sdlTransform.m12, 0.0),
		getOrDefault(sdlTransform.m13, 0.0),
		getOrDefault(sdlTransform.m21, 0.0),
		getOrDefault(sdlTransform.m22, 1.0),
		getOrDefault(sdlTransform.m23, 0.0),
		getOrDefault(sdlTransform.m31, 0.0),
		getOrDefault(sdlTransform.m32, 0.0),
		getOrDefault(sdlTransform.m33, 1.0)};
}


void CPaintConverters::ToSdlTransform(const QTransform& transform, sdl::V1_0::imt2d::CTransform2d& sdlTransform)
{
	sdlTransform.m11 = transform.m11();
	sdlTransform.m12 = transform.m12();
	sdlTransform.m13 = transform.m13();
	sdlTransform.m21 = transform.m21();
	sdlTransform.m22 = transform.m22();
	sdlTransform.m23 = transform.m23();
	sdlTransform.m31 = transform.m31();
	sdlTransform.m32 = transform.m32();
	sdlTransform.m33 = transform.m33();
}


bool CPaintConverters::ToQtBrush(const sdl::V1_0::imt2d::CBrush& sdlBrush, QBrush& brush)
{
	const int alpha = sdlBrush.alpha.value_or(255);

	QColor color = Qt::black;
	if (sdlBrush.color) {
		color = CPaintConverters::ToQColor(*sdlBrush.color, alpha);
	}
	else {
		color.setAlpha(alpha);
	}

	Qt::BrushStyle style = Qt::SolidPattern;
	if (sdlBrush.style) {
		style = CPaintConverters::ToQtBrushStyle(*sdlBrush.style);
	}

	brush = QBrush(color, style);
	ApplyGradientToBrush(sdlBrush, brush);

	if (sdlBrush.transform) {
		brush.setTransform(CPaintConverters::ToQtTransform(*sdlBrush.transform));
	}
	ApplyTextureToBrush(sdlBrush, brush);

	return true;
}


void CPaintConverters::ToSdlBrush(const QBrush& brush, sdl::V1_0::imt2d::CBrush& sdlBrush)
{
	const QColor color = brush.color();
	sdlBrush.color.emplace() = CPaintConverters::ToSdlColor(color);
	sdlBrush.alpha = color.alpha();
	sdlBrush.style = CPaintConverters::ToSdlBrushStyle(brush.style());

	if (!brush.transform().isIdentity()) {
		CPaintConverters::ToSdlTransform(brush.transform(), sdlBrush.transform.emplace());
	}

	QImage textureImage = brush.textureImage();
	if (!textureImage.isNull()) {
		QByteArray bytes;
		QBuffer buffer(&bytes);
		if (buffer.open(QIODevice::WriteOnly) && textureImage.save(&buffer, "PNG")) {
			auto& sdlTexture = sdlBrush.texture.emplace();
			sdlTexture.imageFormat = sdl::V1_0::imt2d::ImageFormat::PNG;
			sdlTexture.imageData = QString::fromLatin1(bytes.toBase64());
		}
	}

	if (const QGradient* gradient = brush.gradient()) {
		sdl::V1_0::imt2d::GradientSpread sdlSpread = CPaintConverters::ToSdlGradientSpread(gradient->spread());
		sdl::V1_0::imt2d::GradientCoordinateMode sdlCoordMode = CPaintConverters::ToSdlGradientCoordinateMode(gradient->coordinateMode());

		switch (gradient->type()) {
		case QGradient::LinearGradient: {
			const auto* linear = static_cast<const QLinearGradient*>(gradient);
			sdl::V1_0::imt2d::CLinearGradient sdlLinear;
			ToSdlPoint(linear->start(), sdlLinear.start.emplace());
			ToSdlPoint(linear->finalStop(), sdlLinear.end.emplace());
			sdlLinear.spread = sdlSpread;
			sdlLinear.coordinateMode = sdlCoordMode;
			FillSdlStopsFromQtGradient(*gradient, sdlLinear.stops.emplace());
			sdlBrush.gradient.emplace() = sdl::V1_0::imt2d::BrushGradient(sdlLinear);
			break;
		}
		case QGradient::RadialGradient: {
			const auto* radial = static_cast<const QRadialGradient*>(gradient);
			sdl::V1_0::imt2d::CRadialGradient sdlRadial;
			ToSdlPoint(radial->center(), sdlRadial.center.emplace());
			sdlRadial.radius = radial->radius();
			ToSdlPoint(radial->focalPoint(), sdlRadial.focalPoint.emplace());
			sdlRadial.centerRadius = radial->centerRadius();
			sdlRadial.focalRadius = radial->focalRadius();
			sdlRadial.spread = sdlSpread;
			sdlRadial.coordinateMode = sdlCoordMode;
			FillSdlStopsFromQtGradient(*gradient, sdlRadial.stops.emplace());
			sdlBrush.gradient.emplace() = sdl::V1_0::imt2d::BrushGradient(sdlRadial);
			break;
		}
		case QGradient::ConicalGradient: {
			const auto* conical = static_cast<const QConicalGradient*>(gradient);
			sdl::V1_0::imt2d::CConicalGradient sdlConical;
			ToSdlPoint(conical->center(), sdlConical.center.emplace());
			sdlConical.angle = conical->angle();
			sdlConical.coordinateMode = sdlCoordMode;
			FillSdlStopsFromQtGradient(*gradient, sdlConical.stops.emplace());
			sdlBrush.gradient.emplace() = sdl::V1_0::imt2d::BrushGradient(sdlConical);
			break;
		}
		default:
			break;
		}
	}
}


bool CPaintConverters::ToQtPen(const sdl::V1_0::imt2d::CPen& sdlPen, QPen& pen)
{
	int alpha = 255;
	if (sdlPen.alpha) {
		alpha = *sdlPen.alpha;
	}

	QColor color = Qt::black;
	if (sdlPen.color) {
		color = CPaintConverters::ToQColor(*sdlPen.color, alpha);
	}
	else {
		color.setAlpha(alpha);
	}

	pen = QPen(color);

	if (sdlPen.widthF) {
		pen.setWidthF(*sdlPen.widthF);
	}
	else if (sdlPen.width) {
		pen.setWidth(*sdlPen.width);
	}

	if (sdlPen.style) {
		pen.setStyle(CPaintConverters::ToQtPenStyle(*sdlPen.style));
	}
	if (sdlPen.capStyle) {
		pen.setCapStyle(CPaintConverters::ToQtPenCapStyle(*sdlPen.capStyle));
	}
	if (sdlPen.joinStyle) {
		pen.setJoinStyle(CPaintConverters::ToQtPenJoinStyle(*sdlPen.joinStyle));
	}
	if (sdlPen.miterLimit) {
		pen.setMiterLimit(*sdlPen.miterLimit);
	}
	if (sdlPen.dashOffset) {
		pen.setDashOffset(*sdlPen.dashOffset);
	}
	if (sdlPen.dashPattern) {
		QVector<qreal> pattern;
		for (const double value : *sdlPen.dashPattern) {
			pattern.append(static_cast<qreal>(value));
		}
		if (!pattern.isEmpty()) {
			pen.setDashPattern(pattern);
		}
	}
	if (sdlPen.cosmetic) {
		pen.setCosmetic(*sdlPen.cosmetic);
	}

	if (sdlPen.brush) {
		QBrush brush;
		const bool isFullyConverted = CPaintConverters::ToQtBrush(*sdlPen.brush, brush);
		pen.setBrush(brush);
		return isFullyConverted;
	}

	return true;
}


void CPaintConverters::ToSdlPen(const QPen& pen, sdl::V1_0::imt2d::CPen& sdlPen)
{
	const QColor color = pen.color();
	sdlPen.color.emplace() = CPaintConverters::ToSdlColor(color);
	sdlPen.alpha = color.alpha();
	sdlPen.width = pen.width();
	sdlPen.widthF = pen.widthF();
	sdlPen.style = CPaintConverters::ToSdlPenStyle(pen.style());
	sdlPen.capStyle = CPaintConverters::ToSdlPenCapStyle(pen.capStyle());
	sdlPen.joinStyle = CPaintConverters::ToSdlPenJoinStyle(pen.joinStyle());
	sdlPen.miterLimit = pen.miterLimit();
	sdlPen.dashOffset = pen.dashOffset();
	sdlPen.cosmetic = pen.isCosmetic();

	const QVector<qreal> pattern = pen.dashPattern();
	if (!pattern.isEmpty()) {
		auto& sdlPattern = sdlPen.dashPattern.emplace();
		for (qreal value : pattern) {
			sdlPattern.append(static_cast<double>(value));
		}
	}

	CPaintConverters::ToSdlBrush(pen.brush(), sdlPen.brush.emplace());
}


} // namespace imt2d
