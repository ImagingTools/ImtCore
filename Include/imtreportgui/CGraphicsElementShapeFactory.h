#pragma once


// ImtCore includes
#include <imtreportgui/IGraphicsElementShapeFactory.h>


namespace imtreportgui
{


class CGraphicsElementShapeFactory: virtual public IGraphicsElementShapeFactory
{
public:
	// reimplemented (IGraphicsElementShapeFactory)
	virtual QGraphicsItem* CreateShape(const i2d::IObject2d& object2d) const;
};


} // namespace imtreportgui


