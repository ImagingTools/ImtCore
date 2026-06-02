#pragma once


// ControlsGallery includes
#include <GeneratedFiles/controlsgallerysdl/SDL/1.0/CPP/ParamsSetTest_fwd.h>


namespace controlsgallerygql
{


class CParamsSetTestControllerComp: public sdl::V1_0::controlsgallery::CParamsSetTestGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::controlsgallery::CParamsSetTestGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CParamsSetTestControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::controlsgallery::CParamsSetTestGqlHandlerCompBase)
	sdl::V1_0::imtbase::CParamsSet OnGetParamsSet(const sdl::V1_0::controlsgallery::CGetParamsSetGqlRequest& getParamsSetRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	sdl::V1_0::controlsgallery::CSetParamsSetResult OnSetParamsSet(const sdl::V1_0::controlsgallery::CSetParamsSetGqlRequest& setParamsSetRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
};


} // namespace controlsgallerygql


