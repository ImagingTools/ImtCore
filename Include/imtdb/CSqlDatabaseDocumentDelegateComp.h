// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ifile/IFilePersistence.h>

// ImtCore includes
#include <imtbase/IComplexCollectionFilter.h>
#include <imtbase/IRevisionController.h>
#include <imtdb/CSqlDatabaseObjectDelegateCompBase.h>
#include <imtdb/IJsonBasedMetaInfoDelegate.h>
#include <imtdb/IDependentMetaInfoController.h>
#include <imtdb/ISqlJsonXPathExtractor.h>
#include <imtcol/CDocumentCollectionFilter.h>
#include <imtcol/CDocumentIdFilter.h>


namespace imtdb
{


struct RawSqlExpression
{
	QString sql;
};


class CSqlDatabaseDocumentDelegateComp:
			public imtdb::CSqlDatabaseObjectDelegateCompBase,
			virtual public imtbase::IRevisionController,
			virtual public imtdb::IDependentMetaInfoController
{
public:
	typedef imtdb::CSqlDatabaseObjectDelegateCompBase BaseClass;
	
	static const QByteArray s_idColumn;
	static const QByteArray s_typeIdColumn;
	static const QByteArray s_documentIdColumn;
	static const QByteArray s_nameColumn;
	static const QByteArray s_descriptionColumn;
	static const QByteArray s_documentColumn;
	static const QByteArray s_dataMetaInfoColumn;
	static const QByteArray s_addedColumn;
	static const QByteArray s_lastModifiedColumn;
	static const QByteArray s_stateColumn;
	static const QByteArray s_revisionInfoColumn;
	static const QByteArray s_ownerIdKey;
	static const QByteArray s_ownerNameKey;
	static const QByteArray s_operationDescriptionKey;
	static const QByteArray s_revisionNumberKey;
	static const QByteArray s_checksumKey;
	static QSet<QString> s_filterableColumns;

	I_BEGIN_COMPONENT(CSqlDatabaseDocumentDelegateComp)
		I_REGISTER_INTERFACE(imtbase::IRevisionController);
		I_REGISTER_INTERFACE(imtdb::IDependentMetaInfoController);
		I_ASSIGN(m_useDataMetaInfoAttrPtr, "UseDataMetaInfo", "If true - documents will be searched and sorted by the 'DataMetaInfo' column,\n else - otherwise according to the contents of the 'Document' column", true, false);
		I_ASSIGN_MULTI_0(m_documentFactoriesCompPtr, "DocumentFactories", "Factory list used for creation of the new document instance according to the given type-ID", true);
		I_ASSIGN(m_metaInfoCreatorCompPtr, "MetaInfoCreator", "Creator of metainformation of object data", false, "MetaInfoCreator");
		I_ASSIGN(m_jsonBasedMetaInfoDelegateCompPtr, "JsonBasedMetaInfoDelegate", "Delegate for converting document metainfo to JSON representation", false, "JsonBasedMetaInfoDelegate");
		I_ASSIGN(m_sqlJsonXPathExtractorCompPtr, "SqlJsonXPathExtractor", "SQL json X-Path extractor", false, "SqlJsonXPathExtractor");
	I_END_COMPONENT

	// reimplemented (imtdb::ISqlDatabaseObjectDelegate)
	virtual QByteArray GetCountQuery(const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual QByteArray GetObjectIdFromRecord(const QSqlRecord& record) const override;
	virtual QByteArray GetSelectionQuery(
				const QByteArray& objectId = QByteArray(),
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual istd::IChangeableUniquePtr CreateObjectFromRecord(
				const QSqlRecord& record,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual NewObjectQuery CreateNewObjectQuery(
				const QByteArray& typeId,
				const QByteArray& proposedObjectId,
				const QString& objectName,
				const QString& objectDescription,
				const istd::IChangeable* valuePtr,
				const imtbase::IOperationContext* operationContextPtr) const override;
	virtual QByteArray CreateDeleteObjectsQuery(
				const imtbase::IObjectCollection& collection,
				const imtbase::ICollectionInfo::Ids& objectIds,
				const imtbase::IOperationContext* operationContextPtr) const override;
	virtual QByteArray CreateDeleteObjectSetQuery(
				const imtbase::IObjectCollection& collection,
				const iprm::IParamsSet* paramsPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual QByteArray CreateRestoreObjectsQuery(
				const imtbase::IObjectCollection& collection,
				const imtbase::ICollectionInfo::Ids& objectIds,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual QByteArray CreateRestoreObjectSetQuery(
				const imtbase::IObjectCollection& collection,
				const iprm::IParamsSet* paramsPtr = nullptr,
				const imtbase::IOperationContext* operationContextPtr = nullptr) const override;
	virtual QByteArray CreateUpdateObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const istd::IChangeable& object,
				const imtbase::IOperationContext* operationContextPtr,
				bool useExternDelegate = true) const override;
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
	virtual QByteArray GetSelectionByMetaInfoQuery(const QByteArray& metaInfoId, const QVariant& metaInfoValue) const override;
	virtual QByteArray CreateUpdateMetaInfoQuery(const QSqlRecord& record) const override;

	// reimplemented (imtbase::IRevisionController)
	virtual RevisionInfoList GetRevisionInfoList(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId) const override;
	virtual int BackupRevision(
				const imtbase::IObjectCollection& collection,
				const imtbase::ICollectionInfo::Id& objectId,
				const QString& userComment = QString()) const override;
	virtual bool RestoreRevision(
				imtbase::IObjectCollection& collection,
				const imtbase::ICollectionInfo::Id& objectId,
				int revision) const override;
	virtual bool ExportRevision(
				const imtbase::IObjectCollection& collection,
				const imtbase::ICollectionInfo::Id& objectId,
				int revision,
				const QString& filePath) const override;
	virtual bool DeleteRevision(
				imtbase::IObjectCollection& collection,
				const imtbase::ICollectionInfo::Id& objectId,
				int revision) const override;
	
	// reimplemented (imtdb::IDependentMetaInfoController)
	virtual bool UpdateDependentMetaInfo(const DependentMetaInfo& metaInfo) const override;
	virtual bool ClearDependentMetaInfo(const MetaFieldCleanupPlan& metaInfo) const override;

protected:
	virtual QByteArray PrepareInsertNewObjectQuery(
				const QByteArray& typeId,
				const QByteArray& objectId,
				const QString& objectName,
				const QString& objectDescription,
				const istd::IChangeable& object,
				const imtbase::IOperationContext* operationContextPtr,
				const QVariant& revisionArgument) const;
	virtual istd::IChangeableUniquePtr CreateObject(const QByteArray& typeId) const;
	virtual bool WriteDataToMemory(const QByteArray& typeId, const istd::IChangeable& object, QByteArray& data) const;
	virtual bool ReadDataFromMemory(const QByteArray& typeId, const QByteArray& data, istd::IChangeable& object) const;
	virtual QByteArray CreateRevisionInfoQuery(const imtbase::IOperationContext* operationContextPtr, const QVariant& revisionArgument, quint32 checksum) const;
	virtual QByteArray CreateJsonBuildObjectQuery(const QVariantMap& paramMap) const;
	virtual QString CreateJsonExtractSql(const QString& jsonName, const QString& key, QMetaType::Type metaType = QMetaType::QString, const QString& tableAlias = QString()) const;

	// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)
	virtual QString GetBaseSelectionQuery() const override;
	virtual idoc::MetaInfoPtr CreateObjectMetaInfo(const QByteArray& typeId) const override;
	virtual bool SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const override;
	virtual bool CreateObjectFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const override;
	virtual bool CreateSortQuery(const imtbase::ICollectionFilter& collectionFilter, QString& sortQuery) const override;
	virtual bool CreateSortQuery(const imtbase::IComplexCollectionFilter& collectionFilter, QString& sortQuery) const override;
	virtual bool CreateFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const override;
	virtual bool CreateTextFilterQuery(const imtbase::ICollectionFilter& collectionFilter, QString& textFilterQuery) const override;
	virtual bool CreateTimeFilterQuery(const imtbase::ITimeFilterParam& timeFilter, QString& timeFilterQuery, const QString& timeFieldId = QStringLiteral("root1.\"TimeStamp\"")) const override;
	
protected:
	virtual bool CreateObjectFilterQuery(const imtbase::IComplexCollectionFilter& collectionFilter, QString& filterQuery) const override;
	virtual bool CreateTextFilterQuery(const imtbase::IComplexCollectionFilter& collectionFilter, QString& textFilterQuery) const override;
	virtual void SubstituteFieldIds(QString& query, bool castToStr = true) const;
	virtual QByteArray GetObjectSelectionQuery(const QByteArray& objectId, const iprm::IParamsSet* paramsPtr = nullptr) const;
	virtual QByteArray CreateJoinTablesQuery() const;
	virtual QByteArray GetCustomColumnsQuery() const;
	virtual bool CreateDocumentCollectionFilterQuery(const imtcol::IDocumentCollectionFilter& documentCollectionFilter, QString& documentFilterQuery) const;
	virtual bool CreateDocumentIdFilterQuery(const imtcol::IDocumentIdFilter& documentIdFilter, QString& documentIdFilterQuery) const;
	bool IsArrayOperation(const QString& query, const QString& field) const;

protected:
	I_ATTR(bool, m_useDataMetaInfoAttrPtr);
	I_MULTIFACT(istd::IChangeable, m_documentFactoriesCompPtr);
	I_REF(imtbase::IMetaInfoCreator, m_metaInfoCreatorCompPtr);
	I_REF(imtdb::IJsonBasedMetaInfoDelegate, m_jsonBasedMetaInfoDelegateCompPtr);
	I_REF(imtdb::ISqlJsonXPathExtractor, m_sqlJsonXPathExtractorCompPtr);
};


} // namespace imtdb


Q_DECLARE_METATYPE(imtdb::RawSqlExpression);


