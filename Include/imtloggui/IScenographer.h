#pragma once


// Qt includes
#include <QtWidgets/QGraphicsScene>

// Acf includes
#include <istd/IPolymorphic.h>


namespace imtloggui
{


class IScenographer: virtual public istd::IPolymorphic
{
public:
	virtual void AddItemsToScene() = 0;
	virtual void RemoveItemsFromScene() = 0;
};


} // namespace imtloggui


