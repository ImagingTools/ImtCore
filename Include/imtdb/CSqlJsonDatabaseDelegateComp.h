#pragma once


// ImtCore includes
#include <imtdb/CSqlDatabaseDocumentDelegateComp.h>
#include <imtbase/CTreeItemModel.h>


namespace imtdb
{


class CSqlJsonDatabaseDelegateComp: public imtdb::CSqlDatabaseDocumentDelegateComp
{
public:
	typedef imtdb::CSqlDatabaseDocumentDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CSqlJsonDatabaseDelegateComp)
		I_ASSIGN(m_isMultiTypeAttrPtr, "IsMultiType", "Is document multitype supported", true, false);
	I_END_COMPONENT

	// reimplemented (imtdb::ISqlDatabaseObjectDelegate)
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
	virtual QByteArray CreateDescriptionObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const QString& description,
				const imtbase::IOperationContext* operationContextPtr) const override;
	virtual QByteArray GetCountQuery(const iprm::IParamsSet* paramsPtr = nullptr) const override;

	// reimplemented (imtdb::CSqlDatabaseDocumentDelegateComp)
	virtual QString GetBaseSelectionQuery() const override;
	virtual bool SetCollectionItemMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const override;
	virtual bool SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const override;
	virtual bool CreateSortQuery(const imtbase::ICollectionFilter& collectionFilter, QString& sortQuery) const override;
	virtual bool CreateFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const override;
	virtual bool CreateObjectFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const override;
	virtual bool CreateTextFilterQuery(const imtbase::ICollectionFilter& collectionFilter, QString& textFilterQuery) const override;
	virtual bool CreateTimeFilterQuery(const imtbase::ITimeFilterParam& timeFilter, QString& timeFilterQuery) const override;
	virtual idoc::MetaInfoPtr CreateObjectMetaInfo(const QByteArray& typeId) const override;

protected:
	virtual QByteArray CreateOperationDescriptionQuery(const QByteArray& objectId, const imtbase::IOperationContext* operationContextPtr) const;
	virtual QByteArray CreateObjectHistoryQuery(
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* paramsPtr = nullptr) const;

	virtual bool WriteDataToMemory(const QByteArray& typeId, const istd::IChangeable& object, QByteArray& data) const override;
	virtual bool ReadDataFromMemory(const QByteArray& typeId, const QByteArray& data, istd::IChangeable& object) const override;
	virtual bool TableIsExists(const QString& tableName) const;

protected:
	I_ATTR(bool, m_isMultiTypeAttrPtr);
};


} // namespace imtdb


