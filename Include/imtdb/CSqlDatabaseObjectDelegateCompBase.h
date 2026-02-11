// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IOptionsList.h>

// ImtCore includes
#include <imtbase/ICollectionFilter.h>
#include <imtbase/IComplexCollectionFilter.h>
#include <imtbase/ITimeFilterParam.h>
#include <imtdb/ISqlDatabaseObjectDelegate.h>
#include <imtdb/IDatabaseEngine.h>


namespace imtdb
{


class CSqlDatabaseObjectDelegateCompBase:
			public ilog::CLoggerComponentBase,
			virtual public imtdb::ISqlDatabaseObjectDelegate
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CSqlDatabaseObjectDelegateCompBase)
		I_REGISTER_INTERFACE(imtdb::ISqlDatabaseObjectDelegate);
		I_ASSIGN(m_typesCompPtr, "ObjectTypes", "List of object types supported by the related database collection", false, "ObjectTypes");
		I_ASSIGN(m_databaseEngineCompPtr, "DatabaseEngine", "Database engine for SQL queries", true, "DatabaseEngine");
		I_ASSIGN(m_tableSchemaAttrPtr, "TableSchema", "Name of the schema containing the object table", false, "");
		I_ASSIGN(m_tableNameAttrPtr, "TableName", "Name of the object table", true, "");
		I_ASSIGN(m_objectIdColumnAttrPtr, "ObjectIdColumn", "Name of the column containing ID of the object", true, "Id");
		I_ASSIGN(m_objectTypeIdColumnAttrPtr, "ObjectTypeIdColumn", "Name of the column containing type-ID of the object", true, "TypeId");
		I_ASSIGN(m_versionInfoCompPtr, "VersionInfo", "Version info", false, "VersionInfo");
	I_END_COMPONENT

	virtual QString SqlEncode(const QString& sqlQuery) const;

	// reimplemented (imtdb::ISqlDatabaseObjectDelegate)
	virtual const iprm::IOptionsList* GetObjectTypeInfos() const override;
	virtual QByteArray GetObjectTypeId(const QByteArray& objectId) const override;
	virtual QByteArray GetCountQuery(const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual QByteArray GetSelectionQuery(
				const QByteArray& objectId = QByteArray(),
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual QByteArray GetObjectIdFromRecord(const QSqlRecord& record) const override;
	virtual QByteArray GetObjectTypeIdFromRecord(const QSqlRecord& record) const override;
	virtual bool CreateObjectInfoFromRecord(
				const QSqlRecord& record,
				idoc::MetaInfoPtr& objectMetaInfoPtr,
				idoc::MetaInfoPtr& collectionItemMetaInfoPtr) const override;
	virtual QVariant GetElementInfoFromRecord(const QSqlRecord& sqlRecord, const QByteArray& infoId) const override;
	virtual QByteArray CreateResetQuery(const imtbase::IObjectCollection& collection) const override;
	virtual QByteArray CreateDataMetaInfoQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr) const override;
	virtual QByteArray CreateCollectionItemMetaInfoQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr) const override;
	virtual QByteArray GetSelectionByMetaInfoQuery(const QByteArray& metaInfoId, const QVariant& metaInfoValue) const override;
	virtual QByteArray CreateUpdateMetaInfoQuery(const QSqlRecord& record) const override;
	virtual QByteArray GetTableName() const override;
	virtual QByteArray GetTableScheme() const override;
	virtual QByteArray CreateRestoreObjectsQuery(
				const imtbase::IObjectCollection& collection,
				const imtbase::ICollectionInfo::Ids& objectIds,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual QByteArray CreateRestoreObjectSetQuery(
				const imtbase::IObjectCollection& collection,
				const iprm::IParamsSet* paramsPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;

protected:
	virtual QString GetBaseSelectionQuery() const;
	virtual idoc::IDocumentMetaInfo* CreateCollectionItemMetaInfo(const QByteArray& typeId) const;
	virtual bool SetCollectionItemMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const;
	virtual idoc::MetaInfoPtr CreateObjectMetaInfo(const QByteArray& typeId) const;
	virtual bool SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const;
	virtual bool CreatePaginationQuery(int offset, int count, QByteArray& paginationQuery) const;
	virtual bool CreateFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const;
	virtual bool CreateObjectFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const;
	virtual bool CreateObjectFilterQuery(const imtbase::IComplexCollectionFilter& collectionFilter, QString& filterQuery) const;
	virtual bool CreateTextFilterQuery(const imtbase::ICollectionFilter& collectionFilter, QString& textFilterQuery) const;
	virtual bool CreateTextFilterQuery(const imtbase::IComplexCollectionFilter& collectionFilter, QString& textFilterQuery) const;
	virtual bool CreateTimeFilterQuery(const imtbase::ITimeFilterParam& timeFilter, QString& timeFilterQuery, const QString& timeFieldId = QStringLiteral("root1.\"TimeStamp\"")) const;
	virtual QString CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const;
	virtual bool CreateSortQuery(const imtbase::ICollectionFilter& collectionFilter, QString& sortQuery) const;
	virtual bool CreateSortQuery(const imtbase::IComplexCollectionFilter& collectionFilter, QString& sortQuery) const;
	virtual QString EncodeTextArgument(const QString& argument) const;
	virtual bool TableExists(const QString& tableName) const;

protected:
	I_REF(imtdb::IDatabaseEngine, m_databaseEngineCompPtr);
	I_REF(iprm::IOptionsList, m_typesCompPtr);
	I_REF(iser::IVersionInfo, m_versionInfoCompPtr);
	I_ATTR(QByteArray, m_tableSchemaAttrPtr);
	I_ATTR(QByteArray, m_tableNameAttrPtr);
	I_ATTR(QByteArray, m_objectIdColumnAttrPtr);
	I_ATTR(QByteArray, m_objectTypeIdColumnAttrPtr);
};


} // namespace imtdb


