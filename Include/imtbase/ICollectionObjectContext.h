#pragma once


// ACF includes
#include <istd/IChangeable.h>


class QByteArray;


namespace imtbase
{
	class IObjectCollection;
}


namespace imtbase
{


class ICollectionObjectContext: virtual public istd::IChangeable
{
public:
	/**
		Get collection in which the object is located.
	*/
	virtual const imtbase::IObjectCollection* GetObjectCollectionPtr() const = 0;

	/**
		Get ID of an object in a collection.
	*/
	virtual QByteArray GetObjectId() const = 0;
};


} // namespace imtbase


