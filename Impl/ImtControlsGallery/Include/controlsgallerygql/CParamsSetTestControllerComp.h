#pragma once


// ControlsGallery includes
#include <GeneratedFiles/controlsgallerysdl/SDL/1.0/CPP/ParamsSetTest.h>


namespace controlsgallerygql
{


class CParamsSetTestControllerComp: public sdl::controlsgallery::ParamsSetTest::CGraphQlHandlerCompBase
{
public:
	typedef sdl::controlsgallery::ParamsSetTest::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CParamsSetTestControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::controlsgallery::ParamsSetTest::CGraphQlHandlerCompBase)
	sdl::imtbase::ImtBaseTypes::CParamsSet OnGetParamsSet(const sdl::controlsgallery::ParamsSetTest::CGetParamsSetGqlRequest& getParamsSetRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	sdl::controlsgallery::ParamsSetTest::CSetParamsSetResult OnSetParamsSet(const sdl::controlsgallery::ParamsSetTest::CSetParamsSetGqlRequest& setParamsSetRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
};


} // namespace controlsgallerygql


