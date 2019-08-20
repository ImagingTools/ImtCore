#pragma once


// ACF includes
#include <iser/ISerializable.h>

// ImtCore includes
#include <imtbase/IObjectCollectionInfo.h>


namespace imtbase
{


class IReferenceCollection: virtual public imtbase::IObjectCollectionInfo, virtual public iser::ISerializable
{
public:
	/**
		Insert new resource into the collection.
	*/
	virtual bool InsertReference(const Id& resourceId) = 0;

	/**
		Remove a given resource from the collection.
	*/
	virtual bool RemoveReference(const Id& resourceId) = 0;

	/**
		Remove all resources from the collection.
	*/
	virtual bool RemoveAllReferences() = 0;
};


} // namespace imtbase


