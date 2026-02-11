// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/TInterfacePtr.h>
#include <iser/ISerializable.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtbase
{


/**
	Interface for an object collection that supports explicit item ordering.
	This facade extends IObjectCollection to allow setting custom order for items.
	\ingroup Collection
*/
class IOrderedObjectCollection: virtual public IObjectCollection, virtual public iser::ISerializable
{
public:
	/**
		Set the position/order of an item in the collection.
		\param itemId		ID of the item to reorder
		\param position		Target position (0-based index) for the item
		\return True if the operation was successful
	*/
	virtual bool SetItemOrder(const Id& itemId, int position) = 0;

	/**
		Get the current position/order of an item in the collection.
		\param itemId		ID of the item
		\return The position (0-based index) of the item, or -1 if not found
	*/
	virtual int GetItemOrder(const Id& itemId) const = 0;

	/**
		Reorder all items by providing a complete ordered list of IDs.
		\param orderedIds	List of item IDs in the desired order
		\return True if the operation was successful
	*/
	virtual bool SetItemsOrder(const Ids& orderedIds) = 0;

	/**
		Get all item IDs in their current custom order if a custom order is active.
		If no custom order is set, the items are returned in the parent collection's default order.
		\return List of item IDs in the effective order (custom order if set, otherwise default order)
	*/
	virtual Ids GetOrderedItemIds() const = 0;

	/**
		Reset the custom ordering to default (typically insertion order or ID order).
		\return True if the operation was successful
	*/
	virtual bool ResetItemOrder() = 0;
};


typedef istd::TUniqueInterfacePtr<IOrderedObjectCollection> IOrderedObjectCollectionUniquePtr;
typedef istd::TSharedInterfacePtr<IOrderedObjectCollection> IOrderedObjectCollectionSharedPtr;


} // namespace imtbase
