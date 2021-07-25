#pragma once

 
// Qt includes
#include <QtSql/QtSql>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtbase/CObjectCollectionBase.h>
#include <imtdb/IDatabaseEngine.h>
#include <imtdb/IDatabaseObjectDelegate.h>


namespace imtdb
{


/**
	Basic implementation of a Database based model.
*/
class CDatabaseObjectCollectionComp: public imtbase::CObjectCollectionBase, public ilog::CLoggerComponentBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;
	typedef imtbase::CObjectCollectionBase BaseClass2;

	I_BEGIN_COMPONENT(CDatabaseObjectCollectionComp);
		I_REGISTER_INTERFACE(CDatabaseObjectCollectionComp);
		I_ASSIGN_MULTI_0(m_objectFactoriesCompPtr, "ObjectFactories", "List of factories used for object creation", false);
		I_ASSIGN_MULTI_0(m_typeIdsAttrPtr, "TypeIds", "List of type-ID corresponding to the registered factories", false);
		I_ASSIGN_MULTI_0(m_typeNamesAttrPtr, "TypeNames", "List of type names corresponding to the registered factories", false);
		I_ASSIGN(m_dbEngineCompPtr, "DatabaseEngine", "Database for getting data", true, "IDatabaseEngine");
		I_ASSIGN(m_updateOnDatabaseConnectedAttrPtr, "UpdateOnConnected", "Sets behavior aftre connected to database \nif true - automatic update", true, false);
		I_ASSIGN(m_selectSqlQueryPathAttrPtr, "SelectSqlQueryPath", "SQL query string file path for Selecting in database", false, "");
		I_ASSIGN(m_insertSqlQueryPathAttrPtr, "InsertSqlQueryPath", "SQL query string file path for Inserting in database", false, "");
		I_ASSIGN(m_updateSqlQueryPathAttrPtr, "UpdateSqlQueryPath", "SQL query string file path for Updating in database", false, "");
		I_ASSIGN(m_deleteSqlQueryPathAttrPtr, "DeleteSqlQueryPath", "SQL query string file path for Deleting in database", false, "");
	I_END_COMPONENT;

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

protected:
	virtual void SyncWithDatabase();
	virtual istd::IChangeable* CreateObjectFromSqlRecord(const QSqlRecord& record) const;

	virtual QSqlQuery ExecSelectSqlQuery(
				 const QVariantMap& bindValues = {},
				 QSqlError* sqlError = nullptr) const;
	virtual QSqlQuery ExecUpdateSqlQuery(
				 const QVariantMap& bindValues,
				 QSqlError* sqlError = nullptr) const;
	 virtual QSqlQuery ExecInsertSqlQuery(
				 const QVariantMap& bindValues,
				 QSqlError* sqlError = nullptr) const;
	virtual QSqlQuery ExecDeleteSqlQuery(
				 const QVariantMap& bindValues,
				 QSqlError* sqlError = nullptr) const;

	QByteArray GetQueryStringFromFile(const QByteArray& filePath) const;

	// reimplemented (CObjectCollectionBase)
	virtual istd::IChangeable* CreateObjectInstance(const QByteArray& typeId) const override;
	virtual void DestroyObjectInstance(istd::IChangeable* objectPtr) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

protected:
	I_MULTIFACT(istd::IChangeable, m_objectFactoriesCompPtr);
	I_MULTIATTR(QByteArray, m_typeIdsAttrPtr);
	I_MULTITEXTATTR(m_typeNamesAttrPtr);
	I_REF(IDatabaseEngine, m_dbEngineCompPtr);
	I_REF(IDatabaseObjectDelegate, m_objectDelegateCompPtr);
	I_ATTR(bool, m_updateOnDatabaseConnectedAttrPtr);
	I_ATTR(QByteArray, m_selectSqlQueryPathAttrPtr);
	I_ATTR(QByteArray, m_insertSqlQueryPathAttrPtr);
	I_ATTR(QByteArray, m_updateSqlQueryPathAttrPtr);
	I_ATTR(QByteArray, m_deleteSqlQueryPathAttrPtr);
};


} // namespace imtdb


