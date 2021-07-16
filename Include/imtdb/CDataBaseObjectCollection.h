#pragma once


// Qt includes
#include <QtSql/QtSql>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtbase/CObjectCollectionBase.h>
#include <imtdb/IDataBaseEngine.h>
#include <imtdb/IDataBaseChangeable.h>


namespace imtdb
{


/**
	Basic implementation of a DataBase based model.
*/
class CDataBaseObjectCollection: public imtbase::CObjectCollectionBase, public ilog::CLoggerComponentBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;
	I_BEGIN_COMPONENT(CDataBaseObjectCollection);
	    I_REGISTER_INTERFACE(CDataBaseObjectCollection);
		I_ASSIGN(m_dbEngineCompPtr, "DataBaseEngine", "DataBase for getting data", true, "IDatabaseEngine");
		I_ASSIGN(m_updateOnDataBaseConnectedAttrPtr, "UpdateOnConnected", "Sets behavior aftre connected to database \nif true - automatic update", true, false);
		I_ASSIGN(m_selectSqlQueryPathAttrPtr, "SelectSqlQueryPath", "SQL query string file path for Selecting in database", false, "");
		I_ASSIGN(m_insertSqlQueryPathAttrPtr, "InsertSqlQueryPath", "SQL query string file path for Inserting in database", false, "");
		I_ASSIGN(m_updateSqlQueryPathAttrPtr, "UpdateSqlQueryPath", "SQL query string file path for Updateing in database", false, "");
		I_ASSIGN(m_deleteSqlQueryPathAttrPtr, "DeleteSqlQueryPath", "SQL query string file path for Deleteing in database", false, "");
	I_END_COMPONENT;

	virtual void Refresh();
	virtual const IDataBaseChangeable* CreateObjectFromSqlRecord(const QSqlRecord& record) const;

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
	static inline std::pair<QString, QVariant> MakeSqlBindBalue(const QByteArray& propName, const IDataBaseChangeable* const object);
	static inline QVariantMap MakeSqlBindBaluesList(const IDataBaseChangeable* const object);

	QByteArray GetQueryStringFromFile(const QByteArray& filePath) const;
	const IDataBaseChangeable* const GetObjectPtrById(const QByteArray& id) const;
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

	// reimplemented (CObjectCollectionBase)
	virtual istd::IChangeable* CreateObjectInstance(const QByteArray& typeId) const override;

protected:
	I_REF(IDataBaseEngine, m_dbEngineCompPtr);
	I_ATTR(bool, m_updateOnDataBaseConnectedAttrPtr);
	I_ATTR(QByteArray, m_selectSqlQueryPathAttrPtr);
	I_ATTR(QByteArray, m_insertSqlQueryPathAttrPtr);
	I_ATTR(QByteArray, m_updateSqlQueryPathAttrPtr);
	I_ATTR(QByteArray, m_deleteSqlQueryPathAttrPtr);
};


} // namespace imtdb


