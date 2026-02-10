// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ibase/IApplicationInfo.h>

// ImtCore includes
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/UserActions.h>


namespace imtauthgql
{


class CUserActionCollectionControllerComp: public sdl::imtauth::UserActions::CUserActionCollectionControllerCompBase
{
public:
	typedef sdl::imtauth::UserActions::CUserActionCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CUserActionCollectionControllerComp);
		I_ASSIGN(m_applicationInfoCompPtr, "ApplicationInfo", "Application info", true, "ApplicationInfo");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtauth::UserActions::CUserCollectionControllerCompBase )
	virtual bool CreateRepresentationFromObject(
				const ::imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::imtauth::UserActions::CGetUserActionsGqlRequest& getUserActionsRequest,
				sdl::imtauth::UserActions::CUserActionItemData::V1_0& representationObject,
				QString& errorMessage) const override;
	void SetAdditionalFilters(
				const imtgql::CGqlRequest& gqlRequest,
				const imtgql::CGqlParamObject& viewParamsGql,
				iprm::CParamsSet* filterParamsPtr) const override;

private:
	I_REF(ibase::IApplicationInfo, m_applicationInfoCompPtr);
};


} // namespace imtauth


