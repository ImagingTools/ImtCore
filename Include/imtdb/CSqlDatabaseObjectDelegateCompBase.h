#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IOptionsList.h>

// ImtCore includes
#include <imtbase/ICollectionFilter.h>
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
		I_ASSIGN(m_tableNameAttrPtr, "TableName", "Name of the object table", true, "");
		I_ASSIGN(m_objectIdColumnAttrPtr, "ObjectIdColumn", "Name of the column containing ID of the object", true, "Id");
		I_ASSIGN(m_objectTypeIdColumnAttrPtr, "ObjectTypeIdColumn", "Name of the column containing type-ID of the object", true, "TypeId");
		I_ASSIGN(m_separatorObjectIdAttrPtr, "SeparatorObjectId", "Separator of the object ID", false, "SeparatorObjectId");
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
protected:
	virtual QString GetBaseSelectionQuery() const;
	virtual idoc::IDocumentMetaInfo* CreateCollectionItemMetaInfo(const QByteArray& typeId) const;
	virtual bool SetCollectionItemMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const;
	virtual idoc::MetaInfoPtr CreateObjectMetaInfo(const QByteArray& typeId) const;
	virtual bool SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const;
	virtual bool CreatePaginationQuery(int offset, int count, QByteArray& paginationQuery) const;
	virtual bool CreateFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const;
	virtual bool CreateObjectFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const;
	virtual bool CreateTextFilterQuery(const imtbase::ICollectionFilter& collectionFilter, QString& textFilterQuery) const;
	virtual bool CreateSortQuery(const imtbase::ICollectionFilter& collectionFilter, QString& sortQuery) const;

protected:
	I_REF(imtdb::IDatabaseEngine, m_databaseEngineCompPtr);
	I_REF(iprm::IOptionsList, m_typesCompPtr);
	I_ATTR(QByteArray, m_tableNameAttrPtr);
	I_ATTR(QByteArray, m_separatorObjectIdAttrPtr);
	I_ATTR(QByteArray, m_objectIdColumnAttrPtr);
	I_ATTR(QByteArray, m_objectTypeIdColumnAttrPtr);
};


} // namespace imtdb


