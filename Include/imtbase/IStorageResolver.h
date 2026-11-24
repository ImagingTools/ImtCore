// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>

// ACF includes
#include <istd/IChangeable.h>
#include <istd/IPolymorphic.h>


namespace imtbase
{


/**
	Interface for resolving of the stored objects by their IDs.
	The resolver encapsulates the knowledge about the place and the way of the object storing
	(in-memory container, object collection, database, external storage etc.).
	It is used by imtbase::TStorageLink to load and save the linked objects.
	The objects are created and deleted by the storage itself, not through the resolver.
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
		Save the object under the given ID. The object must already exist in the storage: the call fails for an unknown ID and never creates an object.
	*/
	virtual bool SaveObject(const Id& objectId, const istd::IChangeable& object) = 0;
};


} // namespace imtbase


