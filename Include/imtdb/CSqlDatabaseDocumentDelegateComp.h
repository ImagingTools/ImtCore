#pragma once


// ACF includes
#include <ifile/IFilePersistence.h>

// ImtCore includes
#include <imtdb/IMetaInfoTableDelegate.h>
#include <imtdb/CSqlDatabaseObjectDelegateCompBase.h>


namespace imtdb
{


class CSqlDatabaseDocumentDelegateComp: public imtdb::CSqlDatabaseObjectDelegateCompBase
{
public:
	typedef imtdb::CSqlDatabaseObjectDelegateCompBase BaseClass;

	I_BEGIN_COMPONENT(CSqlDatabaseDocumentDelegateComp)
		I_ASSIGN_MULTI_0(m_documentFactoriesCompPtr, "DocumentFactories", "Factory list used for creation of the new document instance according to the given type-ID", true);
		I_ASSIGN_MULTI_0(m_documentPersistenceListCompPtr, "DocumentPersistenceList", "List of persistence components for each type of the document", true);
		I_ASSIGN(m_metaInfoTableDelegateCompPtr, "MetaInfoTableDelegate", "Delegate for the table containing meta-informations for the document type", false, "MetaInfoTableDelegate");
		I_ASSIGN(m_documentContentColumnIdAttrPtr, "DocumntContentColumnId", "ID of the column in the table containing document content", true, "Document");
		I_ASSIGN(m_metaInfoTableNameAttrPtr, "MetaInfoTableName", "Name of the meta-info table", true, "");
		I_ASSIGN(m_revisionsTableNameAttrPtr, "RevisionsTableName", "Name of the table contained document revisions", true, "");
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
protected:
	virtual istd::IChangeable* CreateObject(const QByteArray& typeId) const;
	virtual bool WriteDataToMemory(const QByteArray& typeId, const istd::IChangeable& object, QByteArray& data) const;
	virtual bool ReadDataFromMemory(const QByteArray& typeId, const QByteArray& data, istd::IChangeable& object) const;

	// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)
	virtual QString GetBaseSelectionQuery() const override;
	virtual bool CreateObjectFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const override;
	virtual bool CreateObjectInfoFromRecord(
				const QByteArray& typeId,
				const QSqlRecord& record,
				idoc::MetaInfoPtr& objectMetaInfoPtr,
				idoc::MetaInfoPtr& collectionItemMetaInfoPtr) const;
	virtual idoc::MetaInfoPtr CreateObjectMetaInfo(const QByteArray& typeId) const override;
	virtual bool SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const override;

protected:
	const ifile::IFilePersistence* FindDocumentPersistence(const QByteArray& typeId) const;

protected:
	I_MULTIFACT(istd::IChangeable, m_documentFactoriesCompPtr);
	I_MULTIREF(ifile::IFilePersistence, m_documentPersistenceListCompPtr);
	I_REF(IMetaInfoTableDelegate, m_metaInfoTableDelegateCompPtr);
	I_ATTR(QByteArray, m_documentContentColumnIdAttrPtr);
	I_ATTR(QByteArray, m_metaInfoTableNameAttrPtr);
	I_ATTR(QByteArray, m_revisionsTableNameAttrPtr);
};


} // namespace imtdb


