#pragma once


// ACF includes
#include <i2d/IObject2d.h>
#include <iser/IObject.h>

// Qt includes
#if QT_VERSION >= 0x050000
#include <QtWidgets/QAbstractGraphicsShapeItem>
#else
#include <QtGui/QAbstractGraphicsShapeItem>
#endif


namespace imtreport
{


/**
	Interface for describing a graphic element.
*/
class IGraphicsElement: virtual public i2d::IObject2d
{
public:
	/**
		Get the color used for filling of the object.
	*/
	virtual QColor GetFillColor() const = 0;

	/**
		Set the color used for filling of the object.
	*/
	virtual void SetFillColor(const QColor& fillColor) = 0;

	/**
		Get the color used for drawing the strokes of the object.
	*/
	virtual QColor GetStrokeColor() const = 0;

	/**
		Set the color used for drawing the strokes of the object.
	*/
	virtual void SetStrokeColor(const QColor& strokeColor) = 0;

	/**
		Get the width used for drawing the strokes of the object.
	*/
	virtual double GetStrokeWidth() const = 0;

	/**
		Set the width used for drawing the strokes of the object.
	*/
	virtual void SetStrokeWidth(double strokeWidth) = 0;
};


} // namespace imtreport


