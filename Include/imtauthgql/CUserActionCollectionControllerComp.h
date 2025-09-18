#pragma once


// ImtCore includes
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/UserActions.h>


namespace imtauthgql
{


class CUserActionCollectionControllerComp: public sdl::imtauth::UserActions::CUserActionCollectionControllerCompBase
{
public:
	typedef sdl::imtauth::UserActions::CUserActionCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CUserActionCollectionControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtauth::UserActions::CUserCollectionControllerCompBase )
	virtual bool CreateRepresentationFromObject(
				const ::imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::imtauth::UserActions::CGetUserActionsGqlRequest& getUserActionsRequest,
				sdl::imtauth::UserActions::CUserActionItemData::V1_0& representationObject,
				QString& errorMessage) const override;
};


} // namespace imtauth


