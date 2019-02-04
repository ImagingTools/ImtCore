#pragma once


// ImtCore includes
#include <imtreport/IGraphicsElementShapeFactory.h>
#include <imtreport/CTextTable.h>


namespace imtreport
{


class CGraphicsElementShapeFactory: virtual public IGraphicsElementShapeFactory
{
public:
	// reimplemented (IGraphicsElementShapeFactory)
	virtual QGraphicsItem* CreateShape(const i2d::IObject2d& object2d) const;

private:
	QGraphicsItem* CreateTextTable(const imtreport::CTextTable& table) const;
};


} // namespace imtreportgui


