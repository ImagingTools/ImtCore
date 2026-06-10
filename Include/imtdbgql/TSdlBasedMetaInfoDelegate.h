// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <imod/TModelWrap.h>
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtdb/IJsonBasedMetaInfoDelegate.h>


namespace imtdbgql
{


template<class MetaInfoRepresentation>
class TSdlBasedMetaInfoDelegateImpl;


template<class MetaInfoRepresentation>
class TSdlBasedMetaInfoDelegate: virtual public imtdb::IJsonBasedMetaInfoDelegate
{
public:
	virtual bool ToJsonRepresentation(const idoc::IDocumentMetaInfo& metaInfo, QByteArray& json, const QByteArray& typeId) const override = 0;
	virtual bool FromJsonRepresentation(const QByteArray& json, idoc::IDocumentMetaInfo& metaInfo, const QByteArray& typeId) const override = 0;

protected:
	friend class TSdlBasedMetaInfoDelegateImpl<MetaInfoRepresentation>;

	virtual bool FillRepresentation(MetaInfoRepresentation& metaInfoRepresentation, const idoc::IDocumentMetaInfo& metaInfo, const QByteArray& typeId) const = 0;
	virtual bool FillMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const MetaInfoRepresentation& metaInfoRepresentation, const QByteArray& typeId) const = 0;
};


} // namespace imtdbgql


