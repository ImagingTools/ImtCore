// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtdb/IJsonBasedMetaInfoDelegate.h>


namespace imtdb
{


class CJsonBasedMetaInfoDelegateComp:
			public ilog::CLoggerComponentBase,
			virtual public imtdb::IJsonBasedMetaInfoDelegate
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CJsonBasedMetaInfoDelegateComp);
		I_REGISTER_INTERFACE(imtdb::IJsonBasedMetaInfoDelegate);
	I_END_COMPONENT;

	// reimplemented (imtdb::IJsonBasedMetaInfoDelegate)
	virtual bool ToJsonRepresentation(const idoc::IDocumentMetaInfo& metaInfo, QByteArray& json) const override;
	virtual bool FromJsonRepresentation(const QByteArray& json, idoc::IDocumentMetaInfo& metaInfo) const override;

protected:
	virtual bool FillRepresentation(QJsonObject& representation, const idoc::IDocumentMetaInfo& metaInfo) const = 0;
	virtual bool FillMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const QJsonObject& representation) const = 0;
};


} // namespace imtdb


