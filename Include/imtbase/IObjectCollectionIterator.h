#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtbase
{


/**
	Interface for an object collection iteration.
	\ingroup Collection
*/
class IObjectCollectionIterator: virtual public istd::IPolymorphic
{
public:
	/**
		Move iterator to the next position, if available.
	*/
	virtual bool Next() = 0;

	/**
		Move iterator to the previous position, if available.
	*/
	virtual bool Previous() = 0;

	/**
		Get object-ID at the current iterator position.
	*/
	virtual QByteArray GetObjectId() const = 0;

	/**
		Get object data instance at the current iterator position.
	*/
	virtual bool GetObjectData(IObjectCollection::DataPtr& dataPtr) const = 0;

	/**
		Get object data meta information at the current iterator position.
	*/
	virtual idoc::MetaInfoPtr GetDataMetaInfo() const = 0;

	/**
		Get object data element meta information at the current iterator position for a given information-ID.
	*/
	virtual QVariant GetElementInfo(QByteArray infoId) const = 0;
};


} // namespace imtbase


