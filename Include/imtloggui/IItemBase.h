#pragma once


// ACF Includes
#include <idoc/IDocumentMetaInfo.h>


namespace imtloggui
{


/**
	Interface for base item
*/
class IItemBase: virtual public idoc::IDocumentMetaInfo
{
public:
	/**
		Check collides with other item for given scale
	*/
	virtual bool CollidesWithItem(const IItemBase* otherPtr, double scale) const = 0;
};


} // namespace imtloggui


