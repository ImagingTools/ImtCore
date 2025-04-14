#pragma once


// ImtCore includes
#include <imtdb/CJsonBasedMetaInfoDelegateComp.h>


namespace imtlicgql
{


class CLicenseMetaInfoDelegateComp: public imtdb::CJsonBasedMetaInfoDelegateComp
{
public:
	typedef imtdb::CJsonBasedMetaInfoDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CLicenseMetaInfoDelegateComp);
	I_END_COMPONENT;

protected:
	virtual bool FillRepresentation(QJsonObject& representation, const idoc::IDocumentMetaInfo& metaInfo) const override;
	virtual bool FillMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const QJsonObject& representation) const override;
};


} // namespace imtlicgql


