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
	virtual void SetAdditionalFilters(const imtgql::CGqlRequest& gqlRequest, imtbase::CComplexCollectionFilter& complexFilter) const override;

private:
	I_REF(ibase::IApplicationInfo, m_applicationInfoCompPtr);
};


} // namespace imtauth


