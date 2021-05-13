#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{


/**
	General interface for text based representation controller.
*/
class ITextBasedRepresentationController: virtual public istd::IPolymorphic
{
public:
	/**
		Create textual representation for object.
	*/
	virtual bool CreateTextRepresentation(const istd::IChangeable& object, QString& textRepresentation) const = 0;
};


} // namespace imtbase


