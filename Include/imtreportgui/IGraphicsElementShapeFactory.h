#pragma once


// ImtCore includes
#include <i2d/IObject2d.h>


class QGraphicsItem;


namespace imtreportgui
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
	virtual QGraphicsItem* CreateShape(const i2d::IObject2d& object2d) const = 0;
};


} // namespace imtreportgui


