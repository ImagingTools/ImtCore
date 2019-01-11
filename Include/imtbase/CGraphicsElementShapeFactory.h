#pragma once


// ImtCore includes
#include <imtbase/IGraphicsElementShapeFactory.h>


namespace imtbase
{


class CGraphicsElementShapeFactory: virtual public IGraphicsElementShapeFactory
{
public:
	// reimplemented (IGraphicsElementShapeFactory)
	virtual QGraphicsItem* CreateShape(const IGraphicsElement& graphicsElement) const;
};


} // namespace imtbase


