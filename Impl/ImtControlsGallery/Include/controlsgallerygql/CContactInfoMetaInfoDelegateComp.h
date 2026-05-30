#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtdbgql/TSdlBasedMetaInfoDelegate.h>

// ControlsGallery includes
#include <GeneratedFiles/controlsgallerysdl/SDL/1.0/CPP/ContactInfos_fwd.h>


namespace controlsgallerygql
{


class CContactInfoMetaInfoDelegateComp:
			public ilog::CLoggerComponentBase,
			public imtdbgql::TSdlBasedMetaInfoDelegate<sdl::V1_0::controlsgallery::CContactInfoItemData>
{
public:
	typedef ilog::CLoggerComponentBase BaseClass1;
	typedef imtdbgql::TSdlBasedMetaInfoDelegate<sdl::V1_0::controlsgallery::CContactInfoItemData> BaseClass2;

	I_BEGIN_COMPONENT(CContactInfoMetaInfoDelegateComp);
		I_REGISTER_INTERFACE(imtdb::IJsonBasedMetaInfoDelegate);
	I_END_COMPONENT;

protected:
	// reimplemented (imtdbgql::TSdlBasedMetaInfoDelegate<sdl::V1_0::controlsgallery::CContactInfoItemData>)
	virtual bool FillRepresentation(
				sdl::V1_0::controlsgallery::CContactInfoItemData& metaInfoRepresentation,
				const idoc::IDocumentMetaInfo& metaInfo,
				const QByteArray& typeId) const override;
	virtual bool FillMetaInfo(
				idoc::IDocumentMetaInfo& metaInfo,
				const sdl::V1_0::controlsgallery::CContactInfoItemData& metaInfoRepresentation,
				const QByteArray& typeId) const override;
};


} // namespace controlsgallerygql


