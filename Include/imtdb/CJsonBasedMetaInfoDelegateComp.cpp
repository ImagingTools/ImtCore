// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CJsonBasedMetaInfoDelegateComp.h>


// Qt includes
#include<QtCore/QJsonDocument>
#include<QtCore/QJsonObject>


namespace imtdb
{


// reimplemented (imtdb::IJsonBasedMetaInfoDelegate)

bool CJsonBasedMetaInfoDelegateComp::ToJsonRepresentation(const idoc::IDocumentMetaInfo& metaInfo, QByteArray& json) const
{
	QJsonObject object;
	if (FillRepresentation(object, metaInfo)){
		QJsonDocument document(object);
		json = document.toJson(QJsonDocument::Compact);
		
		return !json.isEmpty();
	}
	
	return false;
}


bool CJsonBasedMetaInfoDelegateComp::FromJsonRepresentation(const QByteArray& json, idoc::IDocumentMetaInfo& metaInfo) const
{
	QJsonParseError error;
	QJsonDocument document = QJsonDocument::fromJson(json, &error);
	if (document.isObject() && error.error == QJsonParseError::NoError){
		QJsonObject object = document.object();
		if (FillMetaInfo(metaInfo, object)){
			return true;
		}
	}
	
	return false;
}


} // namespace imtdb


