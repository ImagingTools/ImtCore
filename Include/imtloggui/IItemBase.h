#pragma once


// Qt includes
#include <QtWidgets/QGraphicsItem>

// ACF includes
#include <idoc/IDocumentMetaInfo.h>


namespace imtloggui
{


/**
	Interface for event item
*/
class IItemBase: virtual public idoc::IDocumentMetaInfo
{
public:
	virtual bool CollidesWithItem(const IItemBase* otherPtr, double scale) const = 0;
};


} // namespace imtloggui


