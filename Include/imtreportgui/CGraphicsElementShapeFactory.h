#pragma once


// ImtCore includes
#include <imtreportgui/IGraphicsElementShapeFactory.h>


namespace imtreportgui
{


class CGraphicsElementShapeFactory: virtual public IGraphicsElementShapeFactory
{
public:
	// reimplemented (IGraphicsElementShapeFactory)
	virtual QGraphicsItem* CreateShape(const imtreport::IGraphicsElement& graphicsElement) const;
};


} // namespace imtreportgui


