#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtbase
{


/**
	Common interface for a data object collection.
	\ingroup Collection
*/
class IObjectCollectionIterator: virtual public istd::IPolymorphic
{
public:
	/**
		Retrieves the next record in the result, if available.
	*/
	virtual bool Next() = 0;

	/**
		Retrieves the previos record in the result, if available.
	*/
	virtual bool Previous() = 0;

	/**
		Get object ID.
	*/
	virtual QByteArray GetObjectId() const = 0;

	/**
		Get object data instance from the current record.
	*/
	virtual bool GetObjectData(IObjectCollection::DataPtr& dataPtr) const = 0;

	/**
		Get object data meta information from the current record.
	*/
	virtual idoc::MetaInfoPtr GetDataMetaInfo() const = 0;

	/**
		Get object data element meta information from the current record with the given infoId.
	*/
	virtual QVariant GetElementInfo(QByteArray infoId) const = 0;
};


} // namespace imtbase


