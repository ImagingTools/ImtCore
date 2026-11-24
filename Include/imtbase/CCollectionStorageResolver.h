// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IStorageResolver.h>
#include <imtbase/IObjectCollection.h>


namespace imtbase
{


/**
	Storage resolver working on top of an object collection.
	The resolver doesn't own the data: all operations are delegated to the collection, which is responsible for the thread safety.
	The collection returns the copy of the stored data, so the resolver has the snapshot semantic: use TStorageLink::Save to write the changes back.
	\ingroup Collection
*/
class CCollectionStorageResolver: virtual public IStorageResolver
{
public:
	CCollectionStorageResolver();

	/**
		\param collection	The collection used as the storage. It must outlive the resolver.
	*/
	explicit CCollectionStorageResolver(IObjectCollection& collection);

	/**
		Set the collection used as the storage. The collection must outlive the resolver.
		Without a collection the resolver finds no objects.
	*/
	void SetCollection(IObjectCollection* collectionPtr);

	// reimplemented (imtbase::IStorageResolver)
	virtual istd::IChangeableSharedPtr LoadObject(const Id& objectId) const override;
	virtual bool SaveObject(const Id& objectId, const istd::IChangeable& object) override;

private:
	IObjectCollection* m_collectionPtr;
};


} // namespace imtbase


