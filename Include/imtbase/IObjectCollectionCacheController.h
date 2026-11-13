// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtbase
{


class IObjectCollection;


/**
	Interface for explicit control over cache invalidation in cached object collections.
	Allows triggering cache clearing independently from updates on the underlying collection.
	\ingroup Collection
*/
class IObjectCollectionCacheController : virtual public istd::IPolymorphic
{
public:
	/**
		Flags a source collection reports to the caches observing it.
	*/
	enum SourceChangeFlags
	{
		/**
			The changes of the source are not reported any more, so nothing read from it may be kept.
		*/
		CF_SOURCE_UNWATCHED = 20000,

		/**
			The changes of the source are reported again.
		*/
		CF_SOURCE_WATCHED
	};

	/**
		Invalidates all cached data (filtered sub-collections and individual object data).
		Subsequent calls to accessors will fetch fresh data from the underlying collection.
		This can be used to force cache refresh in scenarios where the base collection
		did not emit change notifications, but cached data may be stale.
	*/
	virtual void InvalidateCache() = 0;

	/**
		The collection whose data is cached: a reader that must see the current state reads it there.
		It may be a cache itself.
	*/
	virtual const IObjectCollection* GetSourceCollection() const = 0;
};


} // namespace imtbase

