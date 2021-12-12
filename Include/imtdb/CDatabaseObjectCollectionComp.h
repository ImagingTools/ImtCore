#pragma once

 
// Qt includes
#include <QtSql/QtSql>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/COptionsManager.h>

// ImtCore includes
#include <imtbase/CObjectCollectionBase.h>
#include <imtdb/IDatabaseEngine.h>
#include <imtdb/IDatabaseObjectDelegate.h>


namespace imtdb
{


/**
	Basic implementation of a Database based model.
*/
class CDatabaseObjectCollectionComp:
			public imtbase::CObjectCollectionBase,
			public ilog::CLoggerComponentBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;
	typedef imtbase::CObjectCollectionBase BaseClass2;

	I_BEGIN_COMPONENT(CDatabaseObjectCollectionComp);
		I_REGISTER_INTERFACE(imtbase::IObjectCollection);
		I_REGISTER_INTERFACE(imtbase::IObjectCollectionInfo);
		I_REGISTER_INTERFACE(imtbase::ICollectionInfo);
		I_ASSIGN(m_objectFactoryCompPtr, "ObjectFactory", "List of factories used for object creation", false, "ObjectFactory");
		I_ASSIGN(m_typeIdAttrPtr, "TypeId", "Type-ID corresponding to the registered factory", true, "Default");
		I_ASSIGN(m_typeNameAttrPtr, "TypeName", "Type name corresponding to the registered factory", true, "Default");
		I_ASSIGN(m_dbEngineCompPtr, "DatabaseEngine", "Database engine used for low level SQL quering", true, "DatabaseEngine");
		I_ASSIGN(m_objectDelegateCompPtr, "ObjectDelegate", "Database object delegate used for creation of C++ objects from the SQL record", true, "ObjectDelegate");
		I_ASSIGN(m_metaInfoCreatorCompPtr, "MetaInfoCreator", "Meta-info creator", false, "MetaInfoCreator");
		I_ASSIGN(m_updateOnDatabaseConnectedAttrPtr, "UpdateOnConnected", "Sets behavior aftre connected to database \nif true - automatic update", true, false);
		I_ASSIGN(m_selectSqlQueryPathAttrPtr, "SelectSqlQueryPath", "SQL query string file path for Selecting in database", true, "");
		I_ASSIGN(m_insertSqlQueryPathAttrPtr, "InsertSqlQueryPath", "SQL query string file path for Inserting in database", true, "");
		I_ASSIGN(m_updateSqlQueryPathAttrPtr, "UpdateSqlQueryPath", "SQL query string file path for Updating in database", true, "");
		I_ASSIGN(m_deleteSqlQueryPathAttrPtr, "DeleteSqlQueryPath", "SQL query string file path for Deleting in database", true, "");
	I_END_COMPONENT;

	// reimplemented (IObjectCollectionInfo)
	virtual const iprm::IOptionsList* GetObjectTypesInfo() const override;

	// reimplemented (imtbase::IObjectCollection)
	virtual QByteArray InsertNewObject(
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				const istd::IChangeable* defaultValuePtr = nullptr,
				const QByteArray& proposedObjectId = QByteArray(),
				const idoc::IDocumentMetaInfo* dataMetaInfoPtr = nullptr,
				const idoc::IDocumentMetaInfo* collectionItemMetaInfoPtr = nullptr) override;
	virtual bool RemoveObject(const QByteArray& objectId) override;
	virtual bool SetObjectData(const QByteArray& objectId, const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual void SetObjectName(const QByteArray& objectId, const QString& objectName) override;
	virtual void SetObjectDescription(const QByteArray& objectId, const QString& objectDescription) override;

protected:
	virtual bool ExecuteTransaction(const QByteArray& sqlQuery) const;
	virtual void CreateCollectionFromDatabase();
	virtual istd::IChangeable* CreateObjectFromSqlRecord(
				const QSqlRecord& record,
				QByteArray& objectId,
				QString& objectName,
				QString& objectDescription,
				QDateTime& lastModified,
				QDateTime& added) const;
	virtual QSqlQuery ExecSelectSqlQuery(const QVariantMap& bindValues = {}, QSqlError* sqlError = nullptr) const;
	virtual QSqlQuery ExecUpdateSqlQuery(const QVariantMap& bindValues, QSqlError* sqlError = nullptr) const;
	virtual QSqlQuery ExecInsertSqlQuery(const QVariantMap& bindValues, QSqlError* sqlError = nullptr) const;
	virtual QSqlQuery ExecDeleteSqlQuery(const QVariantMap& bindValues, QSqlError* sqlError = nullptr) const;
	QByteArray GetQueryStringFromFile(const QByteArray& filePath) const;

	// reimplemented (imtbase::CObjectCollectionBase)
	virtual istd::IChangeable* CreateObjectInstance(const QByteArray& typeId) const override;
	virtual void DestroyObjectInstance(istd::IChangeable* objectPtr) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

protected:
	I_REF(IDatabaseEngine, m_dbEngineCompPtr);

private:
	I_FACT(istd::IChangeable, m_objectFactoryCompPtr);
	I_REF(IDatabaseObjectDelegate, m_objectDelegateCompPtr);
	I_REF(imtbase::IMetaInfoCreator, m_metaInfoCreatorCompPtr);
	I_ATTR(QByteArray, m_typeIdAttrPtr);
	I_TEXTATTR(m_typeNameAttrPtr);
	I_ATTR(bool, m_updateOnDatabaseConnectedAttrPtr);
	I_ATTR(QByteArray, m_selectSqlQueryPathAttrPtr);
	I_ATTR(QByteArray, m_insertSqlQueryPathAttrPtr);
	I_ATTR(QByteArray, m_updateSqlQueryPathAttrPtr);
	I_ATTR(QByteArray, m_deleteSqlQueryPathAttrPtr);

	iprm::COptionsManager m_typesInfo;
};


} // namespace imtdb


