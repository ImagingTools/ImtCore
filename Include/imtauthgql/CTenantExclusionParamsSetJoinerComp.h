// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtservergql/IParamsSetJoiner.h>


namespace imtauthgql
{


/**
 * Params set joiner that injects a tenant exclusion filter.
 *
 * When attached as a ParamsSetJoiner to a CFilterableSelectControllerComp,
 * it adds a "TenantExclusionFilter" param containing the current tenant ID
 * from the GQL context. The collection/database layer should interpret this
 * as "exclude items that belong to the specified tenant".
 *
 * Used for the Invitation workflow where we need to show users who are NOT
 * yet members of the current tenant.
 */
class CTenantExclusionParamsSetJoinerComp:
	public icomp::CComponentBase,
	virtual public imtservergql::IParamsSetJoiner
{
public:
	typedef CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTenantExclusionParamsSetJoinerComp);
		I_REGISTER_INTERFACE(imtservergql::IParamsSetJoiner);
	I_END_COMPONENT;

	// reimplemented (imtservergql::IParamsSetJoiner)
	virtual bool JoinParamsSet(
		const imtgql::CGqlRequest& gqlRequest,
		iprm::IParamsSet& paramsSet) const override;
};


} // namespace imtauthgql
