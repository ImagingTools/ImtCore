// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include<QtCore/QJsonDocument>
#include<QtCore/QJsonObject>

// ACF includes
#include <imod/TModelWrap.h>
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtdb/IJsonBasedMetaInfoDelegate.h>
#include <imtdbgql/TSdlBasedMetaInfoDelegate.h>


namespace imtdbgql
{


template<class MetaInfoRepresentation>
class TSdlBasedMetaInfoDelegateImpl
{
public:
	static bool ToJsonRepresentation(
		const TSdlBasedMetaInfoDelegate<MetaInfoRepresentation>& delegate,
		const idoc::IDocumentMetaInfo& metaInfo,
		QByteArray& json,
		const QByteArray& typeId);
	static bool FromJsonRepresentation(
		const TSdlBasedMetaInfoDelegate<MetaInfoRepresentation>& delegate,
		const QByteArray& json,
		idoc::IDocumentMetaInfo& metaInfo,
		const QByteArray& typeId);
};


template<class MetaInfoRepresentation>
bool TSdlBasedMetaInfoDelegateImpl<MetaInfoRepresentation>::ToJsonRepresentation(
	const TSdlBasedMetaInfoDelegate<MetaInfoRepresentation>& delegate,
	const idoc::IDocumentMetaInfo& metaInfo,
	QByteArray& json,
	const QByteArray& typeId)
{
	MetaInfoRepresentation representation;
	if (delegate.FillRepresentation(representation, metaInfo, typeId)){
		QJsonObject object;

		if (representation.WriteToJsonObject(object)){
			QJsonDocument document(object);
			json = document.toJson(QJsonDocument::Compact);

			return !json.isEmpty();
		}
	}

	return false;
}


template<class MetaInfoRepresentation>
bool TSdlBasedMetaInfoDelegateImpl<MetaInfoRepresentation>::FromJsonRepresentation(
	const TSdlBasedMetaInfoDelegate<MetaInfoRepresentation>& delegate,
	const QByteArray& json,
	idoc::IDocumentMetaInfo& metaInfo,
	const QByteArray& typeId)
{
	QJsonDocument document;
	QJsonParseError error;
	document = document.fromJson(json, &error);
	if (document.isObject() && error.error == QJsonParseError::NoError){
		QJsonObject object = document.object();
		MetaInfoRepresentation representation;
		if (representation.ReadFromJsonObject(object)){
			if (delegate.FillMetaInfo(metaInfo, representation, typeId)){
				return true;
			}
		}
	}

	return false;
}



} // namespace imtdbgql


