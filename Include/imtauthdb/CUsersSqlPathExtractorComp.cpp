// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CUsersSqlPathExtractorComp.h>



namespace imtauthdb
{


// public methods

// reimplemented (ISqlJsonXPathExtractor)

QString CUsersSqlPathExtractorComp::ExtractXPath(
			const QString& jsonKey,
			const QString& fieldId,
			QMetaType::Type metaType,
			const QString& tableAlias) const
{
	if (fieldId == QByteArrayLiteral("SystemId")){
		return QString(R"(COALESCE(root."Document"->'SystemInfos'->0->>'SystemId', ''))");
	}
	
	return BaseClass::ExtractXPath(jsonKey, fieldId, metaType, tableAlias);
}


} // namespace imtauthdb


