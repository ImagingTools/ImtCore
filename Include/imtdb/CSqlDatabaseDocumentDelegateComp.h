#pragma once


// ACF includes
#include <ifile/IFilePersistence.h>

// ImtCore includes
#include <imtbase/IComplexCollectionFilter.h>
#include <imtbase/IRevisionController.h>
#include <imtdb/CSqlDatabaseObjectDelegateCompBase.h>
#include <imtdb/IJsonBasedMetaInfoDelegate.h>


namespace imtdb
{


class CSqlDatabaseDocumentDelegateComp:
			public imtdb::CSqlDatabaseObjectDelegateCompBase,
			virtual public imtbase::IRevisionController
{
public:
	typedef imtdb::CSqlDatabaseObjectDelegateCompBase BaseClass;

	I_BEGIN_COMPONENT(CSqlDatabaseDocumentDelegateComp)
		I_REGISTER_INTERFACE(imtbase::IRevisionController);
		I_ASSIGN(m_useBase64AttrPtr, "UseDocumentBase64Encoding", "", true, true);
		I_ASSIGN_MULTI_0(m_documentFactoriesCompPtr, "DocumentFactories", "Factory list used for creation of the new document instance according to the given type-ID", true);
		I_ASSIGN_MULTI_0(m_documentPersistenceListCompPtr, "DocumentPersistenceList", "List of persistence components for each type of the document", true);
		I_ASSIGN(m_metaInfoCreatorCompPtr, "MetaInfoCreator", "Creator of metainformation of object data", false, "MetaInfoCreator");
		I_ASSIGN(m_jsonBasedMetaInfoDelegateCompPtr, "JsonBasedMetaInfoDelegate", "Delegate for converting document metainfo to JSON representation", false, "JsonBasedMetaInfoDelegate");
	I_END_COMPONENT

	// reimplemented (imtdb::ISqlDatabaseObjectDelegate)
	virtual QByteArray GetCountQuery(const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual QByteArray GetSelectionQuery(
				const QByteArray& objectId = QByteArray(),
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual istd::IChangeable* CreateObjectFromRecord(const QSqlRecord& record) const override;
	virtual NewObjectQuery CreateNewObjectQuery(
				const QByteArray& typeId,
				const QByteArray& proposedObjectId,
				const QString& objectName,
				const QString& objectDescription,
				const istd::IChangeable* valuePtr,
				const imtbase::IOperationContext* operationContextPtr) const override;
	virtual QByteArray CreateDeleteObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const imtbase::IOperationContext* operationContextPtr) const override;
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

protected:
	virtual QByteArray PrepareInsertNewObjectQuery(
				const QByteArray& typeId,
				const QByteArray& objectId,
				const QString& objectName,
				const QString& objectDescription,
				const istd::IChangeable& object,
				const imtbase::IOperationContext* operationContextPtr,
				const QVariant& revisionArgument) const;
	virtual QByteArray CreateOperationDescriptionQuery(const QByteArray& objectId, const imtbase::IOperationContext* operationContextPtr) const;
	virtual istd::IChangeable* CreateObject(const QByteArray& typeId) const;
	virtual bool WriteDataToMemory(const QByteArray& typeId, const istd::IChangeable& object, QByteArray& data) const;
	virtual bool ReadDataFromMemory(const QByteArray& typeId, const QByteArray& data, istd::IChangeable& object) const;

	// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)
	virtual QString GetBaseSelectionQuery() const override;
	virtual idoc::MetaInfoPtr CreateObjectMetaInfo(const QByteArray& typeId) const override;
	virtual bool SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const override;
	virtual bool CreateObjectFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const override;
	virtual bool CreateSortQuery(const imtbase::ICollectionFilter& collectionFilter, QString& sortQuery) const override;
	virtual bool CreateSortQuery(const imtbase::IComplexCollectionFilter& collectionFilter, QString& sortQuery) const override;
	virtual bool CreateFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const override;
	virtual bool CreateTextFilterQuery(const imtbase::ICollectionFilter& collectionFilter, QString& textFilterQuery) const override;
	virtual bool CreateTimeFilterQuery(const imtbase::ITimeFilterParam& timeFilter, QString& timeFilterQuery) const override;

protected:
	virtual bool CreateTextFilterQuery(const imtbase::IComplexCollectionFilter& collectionFilter, QString& textFilterQuery) const;
	const ifile::IFilePersistence* FindDocumentPersistence(const QByteArray& typeId) const;
	void SubstituteFieldIds(QString& query) const;
	virtual QByteArray GetObjectSelectionQuery(const QByteArray& objectId, const iprm::IParamsSet* paramsPtr = nullptr) const;

protected:
	I_ATTR(bool, m_useBase64AttrPtr);
	I_MULTIFACT(istd::IChangeable, m_documentFactoriesCompPtr);
	I_MULTIREF(ifile::IFilePersistence, m_documentPersistenceListCompPtr);
	I_REF(imtbase::IMetaInfoCreator, m_metaInfoCreatorCompPtr);
	I_REF(imtdb::IJsonBasedMetaInfoDelegate, m_jsonBasedMetaInfoDelegateCompPtr);
};


} // namespace imtdb


