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


namespace imtbase
{


/**
	Interface for describing a graphic element.
*/
class IGraphicsElement: virtual public i2d::IObject2d, virtual public iser::IObject
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
	/**
		Set the id. Useful if one wants to distinguish GraphicalElements. E.g. Left element vs. Right Element.
	*/
	virtual void SetId(const QByteArray& id) = 0;
	
	/**
	Get the id. If it is not set, an empty string is returned. Can be used in order to distinguish elements in a chart. E.g. Left element vs. Right Element.
	*/
	virtual QByteArray GetId() const = 0;
};


} // namespace imtbase


