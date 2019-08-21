#pragma once


// ACF includes
#include <iser/ISerializable.h>

// ImtCore includes
#include <imtbase/IObjectCollectionInfo.h>


namespace imtbase
{


/**
	Interface for collection of references to objects which can be identified ( e.g. in a storage) using their IDs.
*/
class IReferenceCollection: virtual public imtbase::IObjectCollectionInfo, virtual public iser::ISerializable
{
public:
	/**
		Insert a new reference into the collection.
	*/
	virtual bool InsertReference(const Id& resourceId) = 0;

	/**
		Remove a given reference from the collection.
	*/
	virtual bool RemoveReference(const Id& resourceId) = 0;

	/**
		Remove all references from the collection.
	*/
	virtual bool RemoveAllReferences() = 0;
};


} // namespace imtbase


