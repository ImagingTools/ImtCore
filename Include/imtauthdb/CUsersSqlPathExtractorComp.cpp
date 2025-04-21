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


