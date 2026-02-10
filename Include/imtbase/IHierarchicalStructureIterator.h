// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IHierarchicalStructureInfo.h>


namespace imtbase
{


/**
	Interface for an object collection iteration.
	\ingroup Collection
*/
class IHierarchicalStructureIterator: virtual public istd::IPolymorphic
{
public:
	/**
		Move iterator to the next position, if available.
	*/
	virtual bool Next() = 0;

	/**
		Move iterator to the previous position, if available.
	*/
	virtual bool Previous() = 0;

	/**
		Get node-ID at the current iterator position.
	*/
	virtual IHierarchicalStructureInfo::Id GetNodeId() const = 0;

	/**
		Get item info at the current iterator position.
	*/
	virtual IHierarchicalStructureInfo::ItemInfo GetItemInfo() const = 0;

	/**
		Get item count for current subquery.
	*/
	virtual int GetItemCount() const = 0;

};


} // namespace imtbase


