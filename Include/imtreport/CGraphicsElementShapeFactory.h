#pragma once


// ImtCore includes
#include <imtreport/IGraphicsElementShapeFactory.h>


namespace imtreport
{


class CGraphicsElementShapeFactory: virtual public IGraphicsElementShapeFactory
{
public:
	// reimplemented (IGraphicsElementShapeFactory)
	virtual QGraphicsItem* CreateShape(const IGraphicsElement& graphicsElement) const;
};


} // namespace imtreport


