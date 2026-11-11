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
	The collection returns the copy of the stored data, so the resolver has the snapshot semantic: use StorageLink::Save to write the changes back.
	One resolver serves objects of a single collection type-ID.
	\ingroup Collection
*/
class CCollectionStorageResolver: virtual public IStorageResolver
{
public:
	/**
		\param collection	The collection used as the storage. It must outlive the resolver.
		\param typeId		Type-ID of the objects to be created in the collection by InsertObject.
	*/
	CCollectionStorageResolver(IObjectCollection& collection, const QByteArray& typeId);

	// reimplemented (imtbase::IStorageResolver)
	virtual istd::IChangeableSharedPtr LoadObject(const Id& objectId) const override;
	virtual bool SaveObject(const Id& objectId, const istd::IChangeable& object) override;
	virtual Id InsertObject(const istd::IChangeableSharedPtr& objectPtr, const QString& name = QString()) override;
	virtual bool RemoveObject(const Id& objectId) override;
	virtual QVariantMap GetStorageParams(const Id& objectId) const override;
	virtual bool IsSnapshotStorage() const override;

protected:
	/**
		Create the resolver without a collection. All operations fail until SetCollection is called.
	*/
	CCollectionStorageResolver();

	/**
		Set the collection used as the storage and the type-ID of the new objects.
		\param collectionPtr	The collection. It is not owned and must outlive the resolver.
	*/
	void SetCollection(IObjectCollection* collectionPtr, const QByteArray& typeId);

private:
	IObjectCollection* m_collectionPtr;
	QByteArray m_typeId;
};


} // namespace imtbase


