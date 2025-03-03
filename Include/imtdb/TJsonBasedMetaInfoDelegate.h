#pragma once


// Qt includes
#include<QtCore/QJsonDocument>
#include<QtCore/QJsonObject>

// ACF includes
#include <imod/TModelWrap.h>
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtdb/IJsonBasedMetaInfoDelegate.h>


namespace imtdbgql
{


template<class MetaInfoRepresentation>
class TJsonBasedMetaInfoDelegate: virtual public imtdb::IJsonBasedMetaInfoDelegate
{
public:
	virtual bool ToJsonRepresentation(const idoc::IDocumentMetaInfo& metaInfo, QByteArray& json) const override;
	virtual bool FromJsonRepresentation(const QByteArray& json, idoc::IDocumentMetaInfo& metaInfo) const override;

protected:
	virtual bool FillRepresentation(QJsonObject& representationObject, const idoc::IDocumentMetaInfo& metaInfo) const = 0;
	virtual bool FillMetaInfo(idoc::IDocumentMetaInfo& metaInfo, QJsonObject& representationObject) const = 0;
};


template<class MetaInfoRepresentation>
bool TSdlBasedMetaInfoDelegate<MetaInfoRepresentation>::ToJsonRepresentation(const idoc::IDocumentMetaInfo& metaInfo, QByteArray& json) const
{
	MetaInfoRepresentation representation;
	if (FillRepresentation(representation, metaInfo)){
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
bool TSdlBasedMetaInfoDelegate<MetaInfoRepresentation>::FromJsonRepresentation(const QByteArray& json, idoc::IDocumentMetaInfo& metaInfo) const
{
	QJsonDocument document;
	QJsonParseError error;
	document = document.fromJson(json, &error);
	if (document.isObject() && error.error == QJsonParseError::NoError){
		QJsonObject object = document.object();
		MetaInfoRepresentation representation;
		if (representation.ReadFromJsonObject(object)){
			if (FillMetaInfo(metaInfo, representation)){
				return true;
			}
		}
	}

	return false;
}



} // namespace imtdbgql


