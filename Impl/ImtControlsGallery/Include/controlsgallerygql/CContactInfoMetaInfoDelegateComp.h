#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtdbgql/TSdlBasedMetaInfoDelegate.h>

// ControlsGallery includes
#include <GeneratedFiles/controlsgallerysdl/SDL/1.0/CPP/ContactInfos.h>


namespace controlsgallerygql
{


class CContactInfoMetaInfoDelegateComp:
			public ilog::CLoggerComponentBase,
			public imtdbgql::TSdlBasedMetaInfoDelegate<sdl::controlsgallery::ContactInfos::CContactInfoItemData::V1_0>
{
public:
	typedef ilog::CLoggerComponentBase BaseClass1;
	typedef imtdbgql::TSdlBasedMetaInfoDelegate<sdl::controlsgallery::ContactInfos::CContactInfoItemData::V1_0> BaseClass2;

	I_BEGIN_COMPONENT(CContactInfoMetaInfoDelegateComp);
		I_REGISTER_INTERFACE(imtdb::IJsonBasedMetaInfoDelegate);
	I_END_COMPONENT;

protected:
	// reimplemented (imtdbgql::TSdlBasedMetaInfoDelegate<sdl::controlsgallery::ContactInfos::CContactInfoItemData>)
	virtual bool FillRepresentation(
				sdl::controlsgallery::ContactInfos::CContactInfoItemData::V1_0& metaInfoRepresentation,
				const idoc::IDocumentMetaInfo& metaInfo,
				const QByteArray& typeId) const override;
	virtual bool FillMetaInfo(
				idoc::IDocumentMetaInfo& metaInfo,
				const sdl::controlsgallery::ContactInfos::CContactInfoItemData::V1_0& metaInfoRepresentation,
				const QByteArray& typeId) const override;
};


} // namespace controlsgallerygql


