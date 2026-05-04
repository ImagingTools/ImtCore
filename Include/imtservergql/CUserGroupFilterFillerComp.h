// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtservergql/IParamsSetJoiner.h>


namespace imtservergql
{


class CUserGroupFilterFillerComp:
	public icomp::CComponentBase,
	virtual public IParamsSetJoiner
{
public:
	typedef CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CUserGroupFilterFillerComp);
		I_REGISTER_INTERFACE(IParamsSetJoiner);
	I_END_COMPONENT;

	// reimplemented (imtservergql::IParamsSetJoiner)
	virtual bool JoinParamsSet(
		const imtgql::CGqlRequest& gqlRequest,
		iprm::IParamsSet& paramsSet) const override;
};


} // namespace imtservergql
