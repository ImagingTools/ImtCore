// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdb/CJsonBasedMetaInfoDelegateComp.h>


namespace imtauthgql
{


class CGroupMetaInfoDelegateComp: public imtdb::CJsonBasedMetaInfoDelegateComp
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGroupMetaInfoDelegateComp);
		I_REGISTER_INTERFACE(imtdb::IJsonBasedMetaInfoDelegate);
	I_END_COMPONENT;

protected:
	virtual bool FillRepresentation(QJsonObject& representation, const idoc::IDocumentMetaInfo& metaInfo) const override;
	virtual bool FillMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const QJsonObject& representation) const override;
};


} // namespace imtauthgql


