#pragma once


// ACF includes
#include <ifile/IFilePersistence.h>

// ImtCore includes
#include <imtdb/ISqlDatabaseTableDelegate.h>
#include <imtdb/CSqlDatabaseObjectDelegateCompBase.h>


namespace imtdb
{


class CSqlDatabaseDocumentDelegateComp: public imtdb::CSqlDatabaseObjectDelegateCompBase
{
public:
	typedef imtdb::CSqlDatabaseObjectDelegateCompBase BaseClass;

	I_BEGIN_COMPONENT(CSqlDatabaseDocumentDelegateComp)
		I_ASSIGN(m_documentFactCompPtr, "DocumentFactory", "Factory used for creation of the new document instance", true, "DocumentFactory");
		I_ASSIGN(m_documentPersistenceCompPtr, "DocumentPersistence", "Persistence for the document", true, "DocumentPersistence");
		I_ASSIGN(m_tableDelegateCompPtr, "TableDelegate", "Delegate for the SQl-table", false, "TableDelegate");
		I_ASSIGN(m_documentContentColumnIdAttrPtr, "DocumntContentColumnId", "ID of the column in the table containing document content", true, "Document");
	I_END_COMPONENT

	// reimplemented (imtdb::ISqlDatabaseObjectDelegate)
	virtual istd::IChangeable* CreateObjectFromRecord(
				const QByteArray& typeId,
				const QSqlRecord& record) const override;
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
	virtual bool WriteDataToMemory(const istd::IChangeable& object, QByteArray& data) const;
	virtual bool ReadDataFromMemory(const QByteArray& data, istd::IChangeable& object) const;

	// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)
	virtual idoc::IDocumentMetaInfo* CreateObjectMetaInfo(const QByteArray& typeId) const override;
	virtual bool SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const override;

private:
	I_FACT(istd::IChangeable, m_documentFactCompPtr);
	I_REF(ifile::IFilePersistence, m_documentPersistenceCompPtr);
	I_REF(ISqlDatabaseTableDelegate, m_tableDelegateCompPtr);
	I_ATTR(QByteArray, m_documentContentColumnIdAttrPtr);
};


} // namespace imtdb


