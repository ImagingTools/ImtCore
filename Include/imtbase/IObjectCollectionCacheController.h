// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


namespace imtbase
{


/**
	Interface for explicit control over cache invalidation in cached object collections.
	Allows triggering cache clearing independently from updates on the underlying collection.
	\ingroup Collection
*/
class IObjectCollectionCacheController : virtual public istd::IPolymorphic
{
public:
	/**
		Invalidates all cached data (filtered sub-collections and individual object data).
		Subsequent calls to accessors will fetch fresh data from the underlying collection.
		This can be used to force cache refresh in scenarios where the base collection
		did not emit change notifications, but cached data may be stale.
	*/
	virtual void InvalidateCache() = 0;
};


} // namespace imtbase

