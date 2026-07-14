#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtdb/IJsonBasedMetaInfoDelegate.h>

// ControlsGallery includes
#include <GeneratedFiles/controlsgallerysdl/SDL/1.0/CPP/ContactInfos.h>


namespace controlsgallerygql
{


class CContactInfoMetaInfoDelegateComp:
			public ilog::CLoggerComponentBase,
			public imtdb::IJsonBasedMetaInfoDelegate
{
public:
	typedef ilog::CLoggerComponentBase BaseClass1;
	typedef imtdb::IJsonBasedMetaInfoDelegate BaseClass2;

	I_BEGIN_COMPONENT(CContactInfoMetaInfoDelegateComp);
		I_REGISTER_INTERFACE(imtdb::IJsonBasedMetaInfoDelegate);
	I_END_COMPONENT;

	virtual bool ToJsonRepresentation(
				const idoc::IDocumentMetaInfo& metaInfo,
				QByteArray& json,
				const QByteArray& typeId) const override;
	virtual bool FromJsonRepresentation(
				const QByteArray& json,
				idoc::IDocumentMetaInfo& metaInfo,
				const QByteArray& typeId) const override;
};


} // namespace controlsgallerygql


