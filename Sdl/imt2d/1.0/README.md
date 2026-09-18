# imt2d 1.0 paint schemas

This directory is split into two SDL modules:
- [PaintSystem.sdl](PaintSystem.sdl): scene graph and shape types.
- [PaintPrimitives.sdl](PaintPrimitives.sdl): paint parameters that are semantically aligned with Qt6 QPen and QBrush.

## Type overview

PaintPrimitives.sdl defines these main groups:

- Pen-related types:
	- PenStyle
	- PenCapStyle
	- PenJoinStyle
	- Pen

- Brush-related types:
	- BrushStyle
	- Brush

- Gradient-related types:
	- GradientSpread
	- GradientCoordinateMode
	- GradientStop
	- LinearGradient
	- RadialGradient
	- ConicalGradient
	- BrushGradient

- Texture/transform helper types:
	- ImageFormat
	- BrushTexture
	- Transform2d

## Qt6 mapping

### Pen mapping (SDL -> Qt)

- Pen.color + Pen.alpha -> QPen::color()
- Pen.width -> QPen::setWidth(int)
- Pen.widthF -> QPen::setWidthF(double)
- Pen.style -> Qt::PenStyle / QPen::setStyle()
- Pen.capStyle -> Qt::PenCapStyle / QPen::setCapStyle()
- Pen.joinStyle -> Qt::PenJoinStyle / QPen::setJoinStyle()
- Pen.miterLimit -> QPen::setMiterLimit()
- Pen.dashPattern -> QPen::setDashPattern()
- Pen.dashOffset -> QPen::setDashOffset()
- Pen.cosmetic -> QPen::setCosmetic()
- Pen.brush -> QPen::setBrush()

### Brush mapping (SDL -> Qt)

- Brush.color + Brush.alpha -> QBrush::color()
- Brush.style -> Qt::BrushStyle / QBrush::setStyle()
- Brush.gradient -> QLinearGradient / QRadialGradient / QConicalGradient
- Brush.texture -> QBrush::setTextureImage()
- Brush.transform -> QBrush::setTransform()

### Gradient mapping (SDL -> Qt)

- GradientStop.position -> QGradientStop first value
- GradientStop.color + GradientStop.alpha -> QGradientStop QColor value
- GradientSpread -> QGradient::Spread
- GradientCoordinateMode -> QGradient::CoordinateMode
- LinearGradient.start/end -> QLinearGradient(start, end)
- RadialGradient.center/radius/focalPoint -> QRadialGradient(center, radius, focalPoint)
- ConicalGradient.center/angle -> QConicalGradient(center, angle)

### Texture/transform mapping (SDL -> Qt)

- BrushTexture.imageData (base64) + BrushTexture.imageFormat -> QImage::loadFromData()
- Transform2d (m11..m33) -> QTransform(m11, m12, m13, m21, m22, m23, m31, m32, m33)

### Type matrix (SDL -> Qt)

| SDL type | Qt type / API | Notes |
| --- | --- | --- |
| [Pen](PaintPrimitives.sdl#L146) | QPen | Main pen container (color, width, style, cap/join, dash, cosmetic, brush). |
| [PenStyle](PaintPrimitives.sdl#L22) | Qt::PenStyle | SOLID_LINE, DASH_LINE, DOT_LINE, DASH_DOT_LINE, DASH_DOT_DOT_LINE, CUSTOM_DASH_LINE map to Qt pen styles. |
| [PenCapStyle](PaintPrimitives.sdl#L33) | Qt::PenCapStyle | FLAT_CAP, SQUARE_CAP, ROUND_CAP. |
| [PenJoinStyle](PaintPrimitives.sdl#L40) | Qt::PenJoinStyle | MITER_JOIN, BEVEL_JOIN, ROUND_JOIN, SVG_MITER_JOIN. |
| [Brush](PaintPrimitives.sdl#L137) | QBrush | Main brush container (color, style, gradient, texture, transform). |
| [BrushStyle](PaintPrimitives.sdl#L48) | Qt::BrushStyle | SOLID_PATTERN, dense/hatch styles, gradient styles, TEXTURE_PATTERN. |
| [GradientSpread](PaintPrimitives.sdl#L71) | QGradient::Spread | PAD_SPREAD, REFLECT_SPREAD, REPEAT_SPREAD. |
| [GradientCoordinateMode](PaintPrimitives.sdl#L78) | QGradient::CoordinateMode | LOGICAL_MODE, STRETCH_TO_DEVICE_MODE, OBJECT_BOUNDING_MODE, OBJECT_MODE. |
| [GradientStop](PaintPrimitives.sdl#L85) | QGradientStop | position + QColor (including alpha). |
| [LinearGradient](PaintPrimitives.sdl#L91) | QLinearGradient | Uses start/end, spread, coordinate mode, stops. |
| [RadialGradient](PaintPrimitives.sdl#L99) | QRadialGradient | Uses center, radius, focalPoint (+ optional center/focal radius fields in SDL). |
| [ConicalGradient](PaintPrimitives.sdl#L110) | QConicalGradient | Uses center, angle, coordinate mode, stops. |
| [BrushGradient](PaintPrimitives.sdl#L117) (union) | QGradient subclasses | SDL variant: LinearGradient or RadialGradient or ConicalGradient. |
| [BrushTexture](PaintPrimitives.sdl#L132) | QImage (via QBrush::setTextureImage) | imageData is base64-encoded binary, imageFormat selects decoder hint. |
| [ImageFormat](PaintPrimitives.sdl#L15) | QImage reader format hint | PNG/JPEG/SVG as load format hints. |
| [Transform2d](PaintPrimitives.sdl#L120) | QTransform | 3x3 matrix with direct element-to-element mapping. |

## Practical rendering rules

Recommended interpretation:
- If both width and widthF are set, prefer widthF as the more precise value.
- Use dashPattern when Pen.style is CUSTOM_DASH_LINE.
- Use Brush.gradient together with LINEAR_GRADIENT_PATTERN, RADIAL_GRADIENT_PATTERN, or CONICAL_GRADIENT_PATTERN.
- Use Brush.texture with TEXTURE_PATTERN.
- Treat Brush.transform as brush-local transform.
- Treat alpha as 0..255 in all places.

## Recommended defaults (when a field is not set)

For stable behavior across renderers, recommended defaults are:
- Pen.style: SOLID_LINE
- Pen.capStyle: SQUARE_CAP
- Pen.joinStyle: BEVEL_JOIN
- Pen.miterLimit: 2.0
- Pen.dashOffset: 0.0
- Pen.cosmetic: false
- Pen.widthF: 1.0
- Brush.style: SOLID_PATTERN
- Gradient.spread: PAD_SPREAD
- Gradient.coordinateMode: LOGICAL_MODE
- Brush.transform: identity 3x3 matrix

### Default values matrix

| SDL field | Recommended default | Qt equivalent |
| --- | --- | --- |
| Pen.style | SOLID_LINE | Qt::SolidLine |
| Pen.capStyle | SQUARE_CAP | Qt::SquareCap |
| Pen.joinStyle | BEVEL_JOIN | Qt::BevelJoin |
| Pen.miterLimit | 2.0 | QPen::miterLimit = 2.0 |
| Pen.dashOffset | 0.0 | QPen::dashOffset = 0.0 |
| Pen.cosmetic | false | QPen::isCosmetic = false |
| Pen.widthF | 1.0 | QPen::widthF = 1.0 |
| Brush.style | SOLID_PATTERN | Qt::SolidPattern |
| Gradient.spread | PAD_SPREAD | QGradient::PadSpread |
| Gradient.coordinateMode | LOGICAL_MODE | QGradient::LogicalMode |
| Brush.transform | identity 3x3 | identity QTransform |

Identity Transform2d:

|      | c1 | c2 | c3 |
| ---  | --- | --- | --- |
| r1   | m11=1 | m12=0 | m13=0 |
| r2   | m21=0 | m22=1 | m23=0 |
| r3   | m31=0 | m32=0 | m33=1 |

## Compatibility

Fields style, gradient, texture, transform, and extended Pen fields are optional.
This keeps older data valid while enabling gradual adoption of new paint capabilities.
