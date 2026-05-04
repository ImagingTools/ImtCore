// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtservergql/ICollectionFilterProvider.h>


namespace imtservergql
{


class CUserGroupFilterProviderComp:
	public icomp::CComponentBase,
	virtual public ICollectionFilterProvider
{
public:
	typedef CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CUserGroupFilterProviderComp);
		I_REGISTER_INTERFACE(ICollectionFilterProvider);
	I_END_COMPONENT;

	// reimplemented (imtservergql::ICollectionFilterProvider)
	virtual bool ApplyCollectionFilters(
		const imtgql::CGqlRequest& gqlRequest,
		iprm::IParamsSet& filterParams) const override;
};


} // namespace imtservergql
