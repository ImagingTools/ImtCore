#pragma once


// Qt includes
#include <QtGui/QColor>

// ACF includes
#include <i2d/IObject2d.h>


namespace imtreport
{


/**
	Interface for describing a graphical representation of a geometry element in 2D-space.
*/
class IGraphicsElement: virtual public i2d::IObject2d
{
public:
	struct GraphicsAttributes
	{
		GraphicsAttributes()
			:fillColor(Qt::black),
			strokeColor(Qt::black),
			strokeWidth(0)
		{
		}

		QColor fillColor;
		QColor strokeColor;
		double strokeWidth;
	};

	/**
		Get common graphics attributes for this object.
	*/
	virtual GraphicsAttributes GetGraphicsAttributes() const = 0;

	/**
		Set common graphics attributes for this object.
	*/
	virtual void SetGraphicsAttributes(const GraphicsAttributes& graphicsAttributes) = 0;
};


} // namespace imtreport


