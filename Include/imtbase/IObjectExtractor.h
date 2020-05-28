#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace imtbase
{


/**
	Interface for an object extractor.
*/
class IObjectExtractor: virtual public istd::IPolymorphic
{
public:
	/**
		Get the list of supported object-IDs.
	*/
	virtual QByteArrayList GetSupportedObjectIds(const istd::IChangeable* compositePtr) const = 0;

	/**
		Extract an object from another (e.g. an object composition).
	*/
	virtual const istd::IChangeable* ExtractObject(const istd::IChangeable* compositePtr, const QByteArray& objectId) const = 0;
};


} // namespace imtbase


