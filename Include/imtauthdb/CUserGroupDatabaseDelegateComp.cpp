#include <imtauthdb/CUserGroupDatabaseDelegateComp.h>


// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtauth/CUserInfoMetaInfo.h>


namespace imtauthdb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

istd::IChangeable* CUserGroupDatabaseDelegateComp::CreateObjectFromRecord(const QSqlRecord& record) const
{
	return nullptr;
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CUserGroupDatabaseDelegateComp::CreateNewObjectQuery(
		const QByteArray& /*typeId*/,
		const QByteArray& /*proposedObjectId*/,
		const QString& objectName,
		const QString& objectDescription,
		const istd::IChangeable* valuePtr) const
{
	return IDatabaseObjectDelegate::NewObjectQuery();
}


QByteArray CUserGroupDatabaseDelegateComp::CreateDeleteObjectQuery(
		const imtbase::IObjectCollection& collection,
		const QByteArray& objectId) const
{
	return QByteArray();
}


QByteArray CUserGroupDatabaseDelegateComp::CreateUpdateObjectQuery(
		const imtbase::IObjectCollection& collection,
		const QByteArray& objectId,
		const istd::IChangeable& object) const
{
	return QByteArray();
}


QByteArray CUserGroupDatabaseDelegateComp::CreateRenameObjectQuery(
		const imtbase::IObjectCollection& collection,
		const QByteArray& objectId,
		const QString& newObjectName) const
{
	return QByteArray();
}


QByteArray CUserGroupDatabaseDelegateComp::CreateDescriptionObjectQuery(
		const imtbase::IObjectCollection& collection,
		const QByteArray& objectId,
		const QString& description) const
{
	return QByteArray();
}


// protected methods

// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)

idoc::MetaInfoPtr CUserGroupDatabaseDelegateComp::CreateObjectMetaInfo(const QByteArray& /*typeId*/) const
{
	return idoc::MetaInfoPtr(new imod::TModelWrap<imtauth::CUserInfoMetaInfo>);
}


bool CUserGroupDatabaseDelegateComp::SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	const istd::IChangeable* instancePtr = CreateObjectFromRecord(record);
	if ((instancePtr != nullptr) && m_metaInfoCreatorCompPtr.IsValid()){
		idoc::MetaInfoPtr retVal;
		if (m_metaInfoCreatorCompPtr->CreateMetaInfo(instancePtr, "GroupInfo", retVal)){
			Q_ASSERT(retVal.IsValid());

			return metaInfo.CopyFrom(*retVal);
		}
	}

	return false;
}


} // namespace imtauthdb


