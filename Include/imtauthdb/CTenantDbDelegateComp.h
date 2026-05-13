// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// ImtCore includes
#include <imtauth/ITenantInfo.h>
#include <imtdb/CSqlDatabaseObjectDelegateCompBase.h>


namespace imtauthdb
{


class CTenantDbDelegateComp: public imtdb::CSqlDatabaseObjectDelegateCompBase
{
public:
	typedef imtdb::CSqlDatabaseObjectDelegateCompBase BaseClass;

	I_BEGIN_COMPONENT(CTenantDbDelegateComp)
		I_ASSIGN(m_tenantFactCompPtr, "TenantFactory", "Factory used for creation of new tenant instances", true, "TenantFactory");
		I_ASSIGN(m_permissionsTableNameAttrPtr, "PermissionsTableName", "Name of the TenantPermissions table", false, "TenantPermissions");
		I_ASSIGN(m_autoCreatePermissionsTableAttrPtr, "AutoCreatePermissionsTable", "Auto-create TenantPermissions table if not exists", false, true);
		I_ASSIGN(m_createPermissionsTableScriptPathAttrPtr, "CreatePermissionsTableScriptPath", "SQL script path for TenantPermissions table creation", false, "CreateTenantPermissionsTable.sql");
	I_END_COMPONENT

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

	// reimplemented (imtdb::ISqlDatabaseObjectDelegate)
	virtual istd::IChangeableUniquePtr CreateObjectFromRecord(
				const QSqlRecord& record,
				const iprm::IParamsSet* dataConfigurationPtr = nullptr) const override;
	virtual NewObjectQuery CreateNewObjectQuery(
				const QByteArray& typeId,
				const QByteArray& proposedObjectId,
				const QString& objectName,
				const QString& objectDescription,
				const istd::IChangeable* valuePtr,
				const imtbase::IOperationContext* operationContextPtr) const override;
	virtual QByteArray CreateUpdateObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const istd::IChangeable& object,
				const imtbase::IOperationContext* operationContextPtr,
				bool useExternDelegate = true) const override;
	virtual QByteArray CreateDeleteObjectsQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArrayList& objectIds,
				const imtbase::IOperationContext* operationContextPtr) const override;
	virtual QByteArray CreateDeleteObjectSetQuery(
				const imtbase::IObjectCollection& collection,
				const iprm::IParamsSet* paramsPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual QByteArray CreateRenameObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const QString& newObjectName,
				const imtbase::IOperationContext* operationContextPtr) const override;
	virtual QByteArray CreateDescriptionObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const QString& description,
				const imtbase::IOperationContext* operationContextPtr) const override;
	virtual bool SetCollectionItemMetaInfoFromRecord(
				const QSqlRecord& record,
				idoc::IDocumentMetaInfo& metaInfo) const override;

	// reimplemented (imtdb::ISqlDatabaseObjectDelegate)
	virtual QByteArray GetSelectionQuery(
				const QByteArray& objectId = QByteArray(),
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;

	// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)
	virtual QByteArray GetCountQuery(const iprm::IParamsSet* paramsPtr = nullptr) const override;

protected:
	// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)
	virtual idoc::MetaInfoPtr CreateObjectMetaInfo(const QByteArray& typeId) const override;
	virtual bool SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const override;
	virtual QString CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const override;

private:
	static QByteArray ExtractUserId(const iprm::IParamsSet* paramsPtr);
	QString GetTenantRelationScopeSubquery(const QByteArray& userId) const;
	bool CreatePermissionsTableIfNeeded();
	QByteArrayList LoadTenantPermissions(const QByteArray& tenantId) const;
	QByteArray CreatePermissionsInsertQuery(const QByteArray& tenantId, const QByteArrayList& permissions) const;
	QByteArray CreatePermissionsDeleteQuery(const QByteArray& tenantId) const;

	I_FACT(imtauth::ITenantInfo, m_tenantFactCompPtr);
	I_ATTR(QByteArray, m_permissionsTableNameAttrPtr);
	I_ATTR(bool, m_autoCreatePermissionsTableAttrPtr);
	I_ATTR(QByteArray, m_createPermissionsTableScriptPathAttrPtr);
};


} // namespace imtauthdb
