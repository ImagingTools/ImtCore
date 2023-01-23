#pragma once


// ImtCore includes
#include <imtdb/CSqlDatabaseObjectDelegateCompBase.h>
#include <imtauth/CRole.h>


namespace imtauthdb
{


class CRoleDatabaseDelegateComp: public imtdb::CSqlDatabaseObjectDelegateCompBase
{
public:
	typedef imtdb::CSqlDatabaseObjectDelegateCompBase BaseClass;

	I_BEGIN_COMPONENT(CRoleDatabaseDelegateComp)
		I_ASSIGN(m_roleInfoFactCompPtr, "Role", "Factory used for creation of the new role instance", true, "Role");
		I_ASSIGN(m_roleCollectionCompPtr, "RoleCollection", "Role collection", true, "RoleCollection");
	I_END_COMPONENT

	// reimplemented (imtdb::ISqlDatabaseObjectDelegate)
	virtual istd::IChangeable* CreateObjectFromRecord(const QSqlRecord& record) const override;
	virtual NewObjectQuery CreateNewObjectQuery(
				const QByteArray& typeId,
				const QByteArray& proposedObjectId,
				const QString& objectName,
				const QString& objectDescription,
				const istd::IChangeable* valuePtr) const override;
	virtual QByteArray CreateDeleteObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId) const override;
	virtual QByteArray CreateUpdateObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const istd::IChangeable& object) const override;
	virtual QByteArray CreateRenameObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const QString& newObjectName) const override;
	virtual QByteArray CreateDescriptionObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const QString& description) const override;
	virtual QByteArray GetObjectIdFromRecord(const QSqlRecord& record) const override;
	virtual QByteArray GetSelectionQuery(
				const QByteArray& objectId = QByteArray(),
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;

protected:
	void GenerateDifferences(
				imtauth::IRole::FeatureIds& currentPermission,
				imtauth::IRole::FeatureIds& newPermissions,
				imtauth::IRole::FeatureIds& addedPermissions,
				imtauth::IRole::FeatureIds& removedPermissions) const;

	// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)
	virtual idoc::MetaInfoPtr CreateObjectMetaInfo(const QByteArray& typeId) const override;
	virtual bool SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const override;

private:
	I_FACT(imtauth::IRole, m_roleInfoFactCompPtr);
	I_REF(imtbase::IObjectCollection, m_roleCollectionCompPtr);
};


} // namespace imtauthdb


