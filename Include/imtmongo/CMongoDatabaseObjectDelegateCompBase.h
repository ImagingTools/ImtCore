#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IOptionsList.h>

// ImtCore includes
#include <imtbase/ICollectionFilter.h>
#include <imtmongo/IMongoDatabaseObjectDelegate.h>
#include <imtmongo/IMongoDatabaseEngine.h>


namespace imtmongo
{


class CMongoDatabaseObjectDelegateCompBase:
			public ilog::CLoggerComponentBase,
			virtual public imtmongo::IMongoDatabaseObjectDelegate
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CMongoDatabaseObjectDelegateCompBase)
		I_REGISTER_INTERFACE(imtmongo::IMongoDatabaseObjectDelegate);
		I_ASSIGN(m_typesCompPtr, "ObjectTypes", "List of object types supported by the related database collection", false, "ObjectTypes");
		I_ASSIGN(m_databaseEngineCompPtr, "DatabaseEngine", "Database engine for SQL queries", true, "DatabaseEngine");
		I_ASSIGN(m_tableNameAttrPtr, "TableName", "Name of the object table", true, "");
		I_ASSIGN(m_objectIdColumnAttrPtr, "ObjectIdColumn", "Name of the column containing ID of the object", true, "Id");
		I_ASSIGN(m_objectTypeIdColumnAttrPtr, "ObjectTypeIdColumn", "Name of the column containing type-ID of the object", true, "TypeId");
		I_ASSIGN(m_separatorObjectIdAttrPtr, "SeparatorObjectId", "Separator of the object ID", false, "SeparatorObjectId");
	I_END_COMPONENT

	// reimplemented (imtdb::IMongoDatabaseObjectDelegate)
	virtual const iprm::IOptionsList* GetObjectTypeInfos() const override;

	virtual QByteArray GetObjectTypeId(const QByteArray& objectId) const override;

	virtual bsoncxx::document::view_or_value GetCountQuery(const iprm::IParamsSet* paramsPtr = nullptr) const override;

	virtual bsoncxx::document::view_or_value GetSelectionQuery(
				const QByteArray& objectId = QByteArray(),
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;

	virtual bsoncxx::document::view_or_value CreateNewObjectQuery(
				const QByteArray& typeId,
				const QByteArray& proposedObjectId,
				const QString& objectName,
				const QString& objectDescription,
				const istd::IChangeable* valuePtr) const = 0;

	virtual bsoncxx::document::view_or_value CreateDeleteObjectQuery(
					const QByteArray& objectId) const = 0;

	virtual bsoncxx::document::view_or_value CreateUpdateObjectQuery(
				const QByteArray& objectId,
				const istd::IChangeable& object,
				bool useExternDelegate = true) const = 0;

	virtual bsoncxx::document::view_or_value CreateResetQuery(const imtbase::IObjectCollection& collection) const override;

	virtual bsoncxx::document::view_or_value CreateDataMetaInfoQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr) const override;

	virtual bsoncxx::document::view_or_value CreateCollectionItemMetaInfoQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr) const override;

	virtual QByteArray GetObjectIdFromRecord(const bsoncxx::document::view& doc) const override;

	virtual bool CreateObjectInfoFromRecord(
				const bsoncxx::document::view& doc,
				idoc::MetaInfoPtr& objectMetaInfoPtr,
				idoc::MetaInfoPtr& collectionItemMetaInfoPtr) const  override;

	virtual istd::IChangeable* CreateObjectFromRecord(const bsoncxx::document::view& doc) const override;

	virtual QVariant GetElementInfoFromRecord(const bsoncxx::document::view& doc, const QString& infoId) const override;

protected:
	virtual QString GetBaseSelectionQuery() const;
	virtual idoc::IDocumentMetaInfo* CreateCollectionItemMetaInfo(const QByteArray& typeId) const;
	virtual idoc::MetaInfoPtr CreateObjectMetaInfo(const QByteArray& typeId) const;
	virtual bool CreatePaginationQuery(int offset, int count, QByteArray& paginationQuery) const;
	virtual bool CreateFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const;
	virtual bool CreateObjectFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const;
	virtual bool CreateTextFilterQuery(const imtbase::ICollectionFilter& collectionFilter, QString& textFilterQuery) const;
	virtual bool CreateSortQuery(const imtbase::ICollectionFilter& collectionFilter, QString& sortQuery) const;

protected:
	I_REF(imtmongo::IMongoDatabaseEngine, m_databaseEngineCompPtr);
	I_REF(iprm::IOptionsList, m_typesCompPtr);
	I_ATTR(QByteArray, m_tableNameAttrPtr);
	I_ATTR(QByteArray, m_separatorObjectIdAttrPtr);
	I_ATTR(QByteArray, m_objectIdColumnAttrPtr);
	I_ATTR(QByteArray, m_objectTypeIdColumnAttrPtr);
};


} // namespace imtmongo


