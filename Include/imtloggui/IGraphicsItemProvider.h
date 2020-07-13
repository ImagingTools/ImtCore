#pragma once


// ImtCore includes
#include <QtWidgets/QGraphicsItem>

// Acf includes
#include <istd/IPolymorphic.h>


namespace imtloggui
{


/**
	Interface for graphics item controller
*/
class IGraphicsItemProvider: virtual public istd::IPolymorphic
{
public:
	/**
		Create graphics item if not exists and insert it to scene
		If graphics item already exists it will returned
	*/
	virtual QGraphicsItem* CreateGraphicsItem() = 0;

	/**
		Delete graphics item and all childrens
	*/
	virtual void DeleteGraphicsItem() = 0;

	/**
		Get graphics item
	*/
	virtual QGraphicsItem* GetGraphicsItem() const = 0;
};


} // namespace imtloggui


