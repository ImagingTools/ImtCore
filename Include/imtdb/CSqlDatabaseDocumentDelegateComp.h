#pragma once


// ACF includes
#include <ifile/IFilePersistence.h>

// ImtCore includes
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
		I_ASSIGN(m_metaInfoCreatorCompPtr, "MetaInfoCreator", "Creator of the document meta infos", false, "MetaInfoCreator");
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
	bool WriteDataToMemory(const istd::IChangeable& object, QByteArray& data) const;
	bool ReadDataFromMemory(const QByteArray& data, istd::IChangeable& object) const;

	// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)
	virtual idoc::IDocumentMetaInfo* CreateObjectMetaInfo(const QByteArray& typeId) const override;
	virtual bool SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const override;

private:
	I_FACT(istd::IChangeable, m_documentFactCompPtr);
	I_REF(ifile::IFilePersistence, m_documentPersistenceCompPtr);
	I_REF(imtbase::IMetaInfoCreator, m_metaInfoCreatorCompPtr);
	I_ATTR(QByteArray, m_documentContentColumnIdAttrPtr);
};


} // namespace imtdb


