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
		Get a pointer to the collection in which the object is located 
	*/
	virtual const imtbase::IObjectCollection* GetObjectCollectionPtr() const = 0;

	/**
		Get ID of an object in a collection
	*/
	virtual QByteArray GetObjectId() const = 0;

	/**
		Set object context
	*/
	virtual void SetObjectContext(const imtbase::IObjectCollection* objectCollectionPtr, const QByteArray& objectId) = 0;
};


} // namespace imtbase


