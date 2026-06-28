// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/ISearchResults.h>


namespace imtbase
{


class ISearchController: virtual public istd::IPolymorphic
{
public:
	virtual QByteArray GetControllerId() const = 0;
	virtual QString GetControllerName() const = 0;

	/**
	 * Returns the total number of matches for the given search text.
	 * This should be efficient (typically just a count query with the filter).
	 *
	 * Called by the search aggregator both for group discovery (to populate totals)
	 * and before returning paged items for the group.
	 */
	virtual int GetMatchCount(const QString& text) const = 0;

	/**
	 * Performs a paged search *within this search group/controller only*.
	 *
	 * Pagination (offset/count) is always relative to this group's results.
	 * The aggregator uses searchResultId to select which controller to call.
	 *
	 * @param text   search query
	 * @param offset 0-based starting index (within this group)
	 * @param count  maximum number of results to return.
	 *               If count <= 0 the implementation may use a default page size.
	 */
	virtual const imtbase::ISearchResults* Search(
		const QString& text, 
		int offset, 
		int count
	) const = 0;
};


} // namespace imtbase


