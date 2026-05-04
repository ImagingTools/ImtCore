// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace imtservergql
{


class ICollectionFilterFiller : virtual public istd::IPolymorphic
{
public:
	/**
		Fill additional filters into the filter params set.
		@param gqlRequest The GraphQL request context.
		@param filterParams The params set to add filters to.
		@return true if filters were successfully applied, false otherwise.
	*/
	virtual bool FillCollectionFilters(
		const imtgql::CGqlRequest& gqlRequest,
		iprm::IParamsSet& filterParams) const = 0;
};


} // namespace imtservergql
