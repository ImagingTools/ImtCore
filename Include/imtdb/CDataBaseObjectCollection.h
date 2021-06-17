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

	explicit CDataBaseObjectCollection();
	virtual ~CDataBaseObjectCollection();

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
				const QByteArray& typeId, ///< = type name
				const QString& name,
				const QString& description,
				const istd::IChangeable* defaultValuePtr = nullptr,
				const QByteArray& preferredId= QByteArray()) override;
	virtual bool RemoveObject(const QByteArray& objectId) override;
	virtual bool SetObjectData(const QByteArray& objectId, const istd::IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual void SetObjectName(const QByteArray& objectId, const QString& objectName) override;
	virtual void SetObjectDescription(const QByteArray& objectId, const QString& objectDescription) override;
	virtual void SetObjectEnabled(const QByteArray& objectId, bool isEnabled = true) override;

protected:
	static inline std::pair<QString, QVariant> MakeSqlBindBalue(const QByteArray& propName, const IDataBaseChangeable* const object);
	static inline QVariantMap MakeSqlBindBaluesList(const IDataBaseChangeable* const object);

	QByteArray GetQueryStringFromFile(const QByteArray& filePath) const;
	const IDataBaseChangeable* const GetObjectPtrById(const QByteArray& id) const;
	Q_REQUIRED_RESULT virtual QSqlQuery ExecSelectSqlQuery(
				_In_opt_ const QVariantMap bindValues = {},
				_Outptr_opt_ QSqlError* sqlError = nullptr) const;
	virtual QSqlQuery ExecUpdateSqlQuery(
				_In_ const QVariantMap bindValues,
				_Outptr_opt_ QSqlError* sqlError = nullptr) const;
	Q_REQUIRED_RESULT virtual QSqlQuery ExecInsertSqlQuery(
				_In_ const QVariantMap bindValues,
				_Outptr_opt_ QSqlError* sqlError = nullptr) const;
	virtual QSqlQuery ExecDeleteSqlQuery(
				_In_opt_ const QVariantMap bindValues,
				_Outptr_opt_ QSqlError* sqlError = nullptr) const;

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


