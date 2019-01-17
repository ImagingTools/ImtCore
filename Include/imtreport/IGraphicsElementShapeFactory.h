#pragma once


// ImtCore includes
#include <imtreport/IGraphicsElement.h>


class QGraphicsItem;


namespace imtreport
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


} // namespace imtreport


