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
class TSdlBasedMetaInfoDelegate: virtual public imtdb::IJsonBasedMetaInfoDelegate
{
public:
	virtual QByteArray ToJsonRepresentation(const idoc::IDocumentMetaInfo& metaInfo) const override;
	virtual idoc::MetaInfoPtr FromJsonRepresentation(const QByteArray& data) const override;

protected:
	virtual bool FillRepresentation(typename MetaInfoRepresentation::V1_0& metaInfoRepresentation, const idoc::IDocumentMetaInfo& metaInfo) const= 0;
	virtual bool FillMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const typename MetaInfoRepresentation::V1_0& metaInfoRepresentation) const= 0;
};


template<class MetaInfoRepresentation>
QByteArray TSdlBasedMetaInfoDelegate<MetaInfoRepresentation>::ToJsonRepresentation(const idoc::IDocumentMetaInfo& metaInfo) const
{
	QByteArray retVal;

	typename MetaInfoRepresentation::V1_0 representation;
	if (FillRepresentation(representation, metaInfo)){
		QJsonObject object;

		if (MetaInfoRepresentation::WriteToJsonObject(representation, object)){
			QJsonDocument document(object);
			retVal = document.toJson(QJsonDocument::Compact);
		}
	}

	return retVal;
}


template<class MetaInfoRepresentation>
idoc::MetaInfoPtr TSdlBasedMetaInfoDelegate<MetaInfoRepresentation>::FromJsonRepresentation(const QByteArray& data) const
{
	QJsonDocument document;
	QJsonParseError error;
	document = document.fromJson(data, &error);
	if (document.isObject() && error.error == QJsonParseError::NoError){
		QJsonObject object = document.object();
		typename MetaInfoRepresentation::V1_0 representation;
		if (MetaInfoRepresentation::ReadFromJsonObject(representation, object)){
			idoc::MetaInfoPtr retVal(new imod::TModelWrap<idoc::CStandardDocumentMetaInfo>());
			if (FillMetaInfo(*retVal, representation)){
				return retVal;
			}
		}
	}

	return idoc::MetaInfoPtr();
}


} // namespace imtdbgql


