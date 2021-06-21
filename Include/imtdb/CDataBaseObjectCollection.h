#pragma once

// Qt includes
#include <QtSql>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtbase/CObjectCollectionBase.h>
#include <imtdb/IDataBaseEngine.h>
#include <imtdb/CDataBaseEngineComp.h>
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
		I_ASSIGN(m_dbEngine, "DataBaseEngine", "DataBase for getting data", true, "IDatabaseEngine");
		I_ASSIGN(m_updateOnDataBaseConnected, "UpdateOnConnected", "Sets behavior aftre connected to database \nif true - automatic update", true, false);
		I_ASSIGN(m_selectSqlQueryPath, "SelectSqlQueryPath", "Sql query string file path for Selecting in database", false, "");
		I_ASSIGN(m_insertSqlQueryPath, "InsertSqlQueryPath", "Sql query string file path for Inserting in database", false, "");
		I_ASSIGN(m_updateSqlQueryPath, "UpdateSqlQueryPath", "Sql query string file path for Updateing in database", false, "");
		I_ASSIGN(m_deleteSqlQueryPath, "DeleteSqlQueryPath", "Sql query string file path for Deleteing in database", false, "");
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
	I_REF(IDatabaseEngine, m_dbEngine);
	I_ATTR(bool, m_updateOnDataBaseConnected);
	I_ATTR(QByteArray, m_selectSqlQueryPath);
	I_ATTR(QByteArray, m_insertSqlQueryPath);
	I_ATTR(QByteArray, m_updateSqlQueryPath);
	I_ATTR(QByteArray, m_deleteSqlQueryPath);
};


} // namespace imtdb


