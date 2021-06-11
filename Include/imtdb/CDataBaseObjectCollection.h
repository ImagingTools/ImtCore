#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtbase/CObjectCollectionBase.h>
#include <imtdb/IDataBaseEngine.h>
#include <imtdb/IDataBaseChangeable.h>


#define IMTDB__C_DATA_BASE_OBJECT_COLLECTION_COMP__INIT \
		I_ASSIGN(m_dbEngine, "DataBaseEngine", "DataBase for getting data", true, "IDatabaseEngine");\
		I_ASSIGN(m_updateOnDataBaseConnected, "UpdateOnConnected", "Sets behavior aftre connected to database \nif true - automatic update", false, true);\
		I_ASSIGN(m_selectSqlQueryString, "SelectSqlQueryString", "Sql query string for Selecting in database", true, "");\
		I_ASSIGN(m_insertSqlQueryString, "InsertSqlQueryString", "Sql query string for Inserting in database", true, "");\
		I_ASSIGN(m_updateSqlQueryString, "UpdateSqlQueryString", "Sql query string for Updateing in database", true, "");\
		I_ASSIGN(m_deleteSqlQueryString, "DeleteSqlQueryString", "Sql query string for Deleteing in database", true, "");

namespace imtdb
{
/**
	Basic implementation of a DataBase based model.
*/
class CDataBaseObjectCollection: public imtbase::CObjectCollectionBase
{
public:
	explicit CDataBaseObjectCollection();
	virtual ~CDataBaseObjectCollection();

	virtual void Refresh();

	static inline std::pair<QString, QVariant> MakeSqlBindBalue(const QByteArray& propName, const IDataBaseChangeable* const object);
	static inline QVariantMap MakeSqlBindBaluesList(const IDataBaseChangeable* const object);

	// reimplemented (imtbase::IObjectCollection)
	virtual QByteArray InsertNewObject(
				const QByteArray& typeId, ///< = type name
				const QString& name,
				const QString& description,
				const istd::IChangeable* defaultValuePtr = nullptr,
				const QByteArray& preferredId= QByteArray()) override = 0;
	virtual bool RemoveObject(const QByteArray& objectId) override = 0;
	virtual bool SetObjectData(const QByteArray& objectId, const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override = 0;
	virtual void SetObjectName(const QByteArray& objectId, const QString& objectName) override = 0;
	virtual void SetObjectDescription(const QByteArray& objectId, const QString& objectDescription) override = 0;
	virtual void SetObjectEnabled(const QByteArray& objectId, bool isEnabled = true) override = 0;

protected:
	const IDataBaseChangeable* const GetObjectPtrById(const QByteArray& id) const;
	Q_REQUIRED_RESULT virtual inline QSqlQuery ExecSelectSqlQuery(
				_In_opt_ QVariantMap bindValues = {},
				_Outptr_opt_ QSqlError* sqlError = nullptr) const;
	virtual inline QSqlQuery ExecUpdateSqlQuery(
				_In_ QVariantMap bindValues,
				_Outptr_opt_ QSqlError* sqlError = nullptr) const;
	Q_REQUIRED_RESULT virtual inline QSqlQuery ExecInsertSqlQuery(
				_In_ QVariantMap bindValues,
				_Outptr_opt_ QSqlError* sqlError = nullptr) const;
	virtual inline QSqlQuery ExecDeleteSqlQuery(
				_In_opt_ QVariantMap bindValues,
				_Outptr_opt_ QSqlError* sqlError = nullptr) const;

	// reimplemented (CObjectCollectionBase)
	virtual istd::IChangeable* CreateObjectInstance(const QByteArray& typeId) const override;

protected:
	I_REF(IDatabaseEngine, m_dbEngine);
	I_ATTR(bool, m_updateOnDataBaseConnected);
	I_ATTR(QByteArray, m_selectSqlQueryString);
	I_ATTR(QByteArray, m_insertSqlQueryString);
	I_ATTR(QByteArray, m_updateSqlQueryString);
	I_ATTR(QByteArray, m_deleteSqlQueryString);
};


} // namespace imtdb


