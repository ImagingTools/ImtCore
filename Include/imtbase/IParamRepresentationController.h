#pragma once


// ACF includes
#include <iser/ISerializable.h>


namespace imtbase
{


/**
	Interface for text based param representation controller.
*/
class IParamRepresentationController: virtual public istd::IPolymorphic
{
public:
	/**
		Create textual representation for param.
	*/
	virtual bool CreateTextRepresentation(const iser::ISerializable& param, QString& textRepresentation) const = 0;
};


} // namespace imtbase


