#pragma once


// ControlsGallery includes
#include <GeneratedFiles/controlsgallerysdl/SDL/1.0/CPP/ParamsSetTest.h>


namespace controlsgallerygql
{


class CParamsSetTestControllerComp: public sdl::V1_0::controlsgallery::CGraphQlHandlerCompBase
{
public:
	typedef sdl::V1_0::controlsgallery::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CParamsSetTestControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::controlsgallery::CGraphQlHandlerCompBase)
	sdl::V1_0::imtbase::CParamsSet OnGetParamsSet(const sdl::V1_0::controlsgallery::CGetParamsSetGqlRequest& getParamsSetRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	sdl::V1_0::controlsgallery::CSetParamsSetResult OnSetParamsSet(const sdl::V1_0::controlsgallery::CSetParamsSetGqlRequest& setParamsSetRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
};


} // namespace controlsgallerygql


