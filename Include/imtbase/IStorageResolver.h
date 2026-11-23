// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/QString>

// ACF includes
#include <istd/IChangeable.h>
#include <istd/IPolymorphic.h>


namespace imtbase
{


/**
	Interface for resolving of the stored objects by their IDs.
	The resolver encapsulates the knowledge about the place and the way of the object storing
	(in-memory container, object collection, database, external storage etc.).
	It is used by imtbase::TStorageLink to access the linked objects.
	\ingroup Collection
*/
class IStorageResolver: virtual public istd::IPolymorphic
{
public:
	using Id = QByteArray;

	/**
		Load the object with the given ID.
		\return Valid pointer to the object or an invalid pointer if the object doesn't exist.
	*/
	virtual istd::IChangeableSharedPtr LoadObject(const Id& objectId) const = 0;

	/**
		Save the object under the given ID. The object must already exist in the storage.
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
};


} // namespace imtbase


