#pragma once


// Qt includes
#include <QtWidgets/QGraphicsScene>

// Qt includes
#include <istd/IPolymorphic.h>


namespace imtloggui
{


/**
	Interface for scene provider
*/
class IGraphicsSceneProvider: virtual public istd::IPolymorphic
{
public:
	/**
		Get graphics scene
	*/
	virtual QGraphicsScene* GetScene() = 0;
};


} // namespace imtloggui


