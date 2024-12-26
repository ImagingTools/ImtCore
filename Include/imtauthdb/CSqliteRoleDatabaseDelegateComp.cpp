#include <imtauthdb/CSqliteRoleDatabaseDelegateComp.h>


// ACF includes
#include <istd/TOptDelPtr.h>
#include <istd/CCrcCalculator.h>

// ImtCore includes
#include <imtauth/CRole.h>


namespace imtauthdb
{


// protected methods

// reimplemented (imtdb::CSqliteJsonDatabaseDelegateComp)

bool CSqliteRoleDatabaseDelegateComp::SetCollectionItemMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	BaseClass::SetCollectionItemMetaInfoFromRecord(record, metaInfo);

	if (record.contains("Document")){
		QByteArray json = record.value("Document").toByteArray();
		QJsonDocument jsonDocument = QJsonDocument::fromJson(json);

		if (!jsonDocument.isNull()){
			QString description = jsonDocument["RoleDescription"].toString();
			metaInfo.SetMetaInfo(imtbase::ICollectionInfo::EIT_DESCRIPTION, description);

			QString name = jsonDocument["RoleName"].toString();
			metaInfo.SetMetaInfo(imtbase::ICollectionInfo::EIT_NAME, name);
		}
	}

	return true;
}


} // namespace imtauthdb


