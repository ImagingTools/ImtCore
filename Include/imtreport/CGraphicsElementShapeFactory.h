#pragma once


// imtreports includes
#include <imtreports/IGraphicsElementShapeFactory.h>


namespace imtreports
{


class CGraphicsElementShapeFactory: virtual public IGraphicsElementShapeFactory
{
public:
	// reimplemented (IGraphicsElementShapeFactory)
	virtual QGraphicsItem* CreateShape(const IGraphicsElement& graphicsElement) const;
};


} // namespace imtreports


