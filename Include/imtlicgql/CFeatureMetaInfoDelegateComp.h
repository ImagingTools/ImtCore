#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtdbgql/TSdlBasedMetaInfoDelegate.h>
#include <GeneratedFiles/imtlicsdl/SDL/1.0/CPP/Features.h>


namespace imtlicgql
{


class CFeatureMetaInfoDelegateComp:
									public ilog::CLoggerComponentBase,
									public imtdbgql::TSdlBasedMetaInfoDelegate<sdl::imtlic::Features::CFeatureData::V1_0>
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CFeatureMetaInfoDelegateComp);
		I_REGISTER_INTERFACE(imtdb::IJsonBasedMetaInfoDelegate);
	I_END_COMPONENT;

protected:
	virtual bool FillRepresentation(sdl::imtlic::Features::CFeatureData::V1_0& metaInfoRepresentation, const idoc::IDocumentMetaInfo& metaInfo) const override;
	virtual bool FillMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const sdl::imtlic::Features::CFeatureData::V1_0& metaInfoRepresentation) const override;
};


} // namespace imtlicgql


