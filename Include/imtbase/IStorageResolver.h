// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QVariantMap>

// ACF includes
#include <istd/IChangeable.h>
#include <istd/IPolymorphic.h>
#include <iser/IVersionInfo.h>


namespace imtbase
{


/**
	Interface for resolving of the stored objects by their IDs.
	The resolver encapsulates the knowledge about the place and the way of the object storing
	(in-memory container, object collection, database, external storage etc.).
	It is used by imtbase::StorageLink to access the linked objects.
	\ingroup Collection
*/
class IStorageResolver: virtual public istd::IPolymorphic
{
public:
	typedef QByteArray Id;

	/**
		Load the object with the given ID.
		\return Valid pointer to the object or an invalid pointer if the object doesn't exist.
		\sa IsSnapshotStorage
	*/
	virtual istd::IChangeableSharedPtr LoadObject(const Id& objectId) const = 0;

	/**
		Save the object under the given ID. The object must already exist in the storage:
		for a missing object the method fails and never creates one.
	*/
	virtual bool SaveObject(const Id& objectId, const istd::IChangeable& object) = 0;

	/**
		Insert a new object into the storage.
		\param objectPtr	Object to be inserted. Storages with the snapshot semantic copy it, other storages can keep the instance.
		\param name			Optional name of the object in the storage.
		\return ID of the new object or an empty ID if the operation failed.
	*/
	virtual Id InsertObject(const istd::IChangeableSharedPtr& objectPtr, const QString& name = QString()) = 0;

	/**
		Remove the object with the given ID from the storage.
	*/
	virtual bool RemoveObject(const Id& objectId) = 0;

	/**
		Get the storage-specific parameters of the object (e.g. table, bucket, URL).
		The set of keys depends on the storage implementation, the key "storage" always contains the storage kind.
	*/
	virtual QVariantMap GetStorageParams(const Id& objectId) const = 0;

	/**
		Returns \c true, if LoadObject returns a copy of the stored data.
		In this case the changes of the loaded object become persistent only after SaveObject.
		If \c false, LoadObject returns the live stored instance.
	*/
	virtual bool IsSnapshotStorage() const = 0;

	/**
		Get the version info used to serialize the objects of this storage.
		StorageLink compares the serialized objects to detect their changes. Without the version info the data of newer versions
		would be skipped, so serialization is not used and the change detection falls back to istd::IChangeable::IsEqual.
		\return Version info or \c nullptr, if not available.
	*/
	virtual const iser::IVersionInfo* GetVersionInfo() const = 0;
};


} // namespace imtbase


