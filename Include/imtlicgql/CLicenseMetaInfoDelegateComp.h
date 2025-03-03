#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtdbgql/TSdlBasedMetaInfoDelegate.h>
#include <GeneratedFiles/imtlicsdl/SDL/1.0/CPP/Licenses.h>


namespace imtlicgql
{


class CLicenseMetaInfoDelegateComp:
									public ilog::CLoggerComponentBase,
									public imtdbgql::TSdlBasedMetaInfoDelegate<sdl::imtlic::Licenses::CLicenseDefinitionData::V1_0>
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CLicenseMetaInfoDelegateComp);
		I_REGISTER_INTERFACE(imtdb::IJsonBasedMetaInfoDelegate);
	I_END_COMPONENT;

protected:
	virtual bool FillRepresentation(sdl::imtlic::Licenses::CLicenseDefinitionData::V1_0& metaInfoRepresentation, const idoc::IDocumentMetaInfo& metaInfo) const override;
	virtual bool FillMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const sdl::imtlic::Licenses::CLicenseDefinitionData::V1_0& metaInfoRepresentation) const override;
};


} // namespace imtlicgql


