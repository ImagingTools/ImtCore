#pragma once


// Qt includes
#include <QtWidgets/QGraphicsScene>

// Acf includes
#include <istd/IPolymorphic.h>


namespace imtloggui
{


class IGraphicsSceneProvider: virtual public istd::IPolymorphic
{
public:
	virtual QGraphicsScene* GetGraphicsScene() = 0;
};


} // namespace imtloggui


