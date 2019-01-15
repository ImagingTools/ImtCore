#pragma once


// Qt includes
#include <QtCore/QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets/QGraphicsItem>
#else
#include <QtGui/QGraphicsItem>
#endif


// GraphicsDocument includes
#include <imtreports/IGraphicsElement.h>


namespace imtreports
{


/**
	Interface for creation of visualization shapes for a graphic element.
*/
class IGraphicsElementShapeFactory: virtual public istd::IPolymorphic
{
public:
	/**
		Create shape item for a graphics element.
	*/
	virtual QGraphicsItem* CreateShape(const IGraphicsElement& graphicsElement) const = 0;
};


} // namespace imtreports


