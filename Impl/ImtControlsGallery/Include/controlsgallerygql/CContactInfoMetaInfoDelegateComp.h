#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtdb/CJsonBasedMetaInfoDelegateComp.h>

namespace controlsgallerygql
{


class CContactInfoMetaInfoDelegateComp:
			public ilog::CLoggerComponentBase,
			public imtdb::CJsonBasedMetaInfoDelegateComp
{
public:
	typedef ilog::CLoggerComponentBase BaseClass1;
	typedef imtdb::CJsonBasedMetaInfoDelegateComp BaseClass2;

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

protected:
	// reimplemented (imtdb::CJsonBasedMetaInfoDelegateComp)
	virtual bool FillRepresentation(
				QJsonObject& representation,
				const idoc::IDocumentMetaInfo& metaInfo,
				const QByteArray& typeId) const override;
	virtual bool FillMetaInfo(
				idoc::IDocumentMetaInfo& metaInfo,
				const QJsonObject& representation,
				const QByteArray& typeId) const override;
};


} // namespace controlsgallerygql
