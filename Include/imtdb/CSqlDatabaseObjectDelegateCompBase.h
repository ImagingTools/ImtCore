#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
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
		I_ASSIGN(m_databaseEngineCompPtr, "DatabaseEngine", "Database engine for SQL queries", true, "DatabaseEngine");
		I_ASSIGN(m_tableNameAttrPtr, "TableName", "Name of the object table", true, "");
		I_ASSIGN(m_objectIdColumnAttrPtr, "ObjectIdColumn", "Name of the column containing ID of the object", true, "Id");
	I_END_COMPONENT

	// reimplemented (imtdb::ISqlDatabaseObjectDelegate)
	virtual QByteArray GetCountQuery(const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual QByteArray GetSelectionQuery(
				const QByteArray& objectId = QByteArray(),
				int offset = -1,
				int count = -1,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual QByteArray GetObjectIdFromRecord(const QByteArray& typeId, const QSqlRecord& record) const override;
	virtual bool CreateObjectInfoFromRecord(
				const QByteArray& typeId,
				const QSqlRecord& record,
				imtbase::IMetaInfoCreator::MetaInfoPtr& objectMetaInfoPtr,
				imtbase::IMetaInfoCreator::MetaInfoPtr& collectionItemMetaInfoPtr) const override;

protected:
	virtual idoc::IDocumentMetaInfo* CreateCollectionItemMetaInfo(const QByteArray& typeId) const;
	virtual bool SetCollectionItemMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const;
	virtual idoc::IDocumentMetaInfo* CreateObjectMetaInfo(const QByteArray& typeId) const;
	virtual bool SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const;

protected:
	I_REF(imtdb::IDatabaseEngine, m_databaseEngineCompPtr);
	I_ATTR(QByteArray, m_tableNameAttrPtr);
	I_ATTR(QByteArray, m_objectIdColumnAttrPtr);
};


} // namespace imtdb


