// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtservergql/IParamsSetJoiner.h>


namespace imtauthgql
{


/**
	Params set joiner that injects a tenant filter in Exclude mode.
	When joined, it creates a CTenantFilterParam with the current tenant from gqlContext
	and sets FilterMode=TFM_EXCLUDE, so that CSqlDatabaseDocumentDelegateComp will use
	NOT EXISTS to exclude users already belonging to the current tenant.
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
