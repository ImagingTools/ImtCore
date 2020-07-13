#pragma once


// Qt includes
#include <QtCore/QRectF>

// ACF includes
#include <istd/IChangeable.h>


namespace imtloggui
{


/**
	Interface for view rect provider
*/
class IViewRectProvider: virtual public istd::IChangeable
{
public:
	/**
		Get the area of the scene that must be visualized
	*/
	virtual QRectF GetViewRect() const = 0;
};


} // namespace imtloggui


