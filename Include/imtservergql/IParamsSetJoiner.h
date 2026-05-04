// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace imtservergql
{


class IParamsSetJoiner : virtual public istd::IPolymorphic
{
public:
	/**
		Join additional parameters into the params set.
		@param gqlRequest The GraphQL request context.
		@param paramsSet The params set to add parameters to.
		@return true if parameters were successfully joined, false otherwise.
	*/
	virtual bool JoinParamsSet(
		const imtgql::CGqlRequest& gqlRequest,
		iprm::IParamsSet& paramsSet) const = 0;
};


} // namespace imtservergql
