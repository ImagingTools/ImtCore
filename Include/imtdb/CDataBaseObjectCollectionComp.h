#pragma once


// std includes
#include <exception>

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
class CDataBaseObjectCollectionComp: public ilog::CLoggerComponentBase,
			public imtbase::CObjectCollectionBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDataBaseObjectCollectionComp);
		I_REGISTER_INTERFACE(imtbase::CObjectCollectionBase);
		I_ASSIGN(m_dbEngine, "DataBaseEngine", "DataBase for getting data", true, "IDatabaseEngine");
		I_ASSIGN(m_updateOnDataBaseConnected, "UpdateOnConnected", "Sets behavior aftre connected to database \nif true - automatic update", false, true);
	I_END_COMPONENT;

	explicit CDataBaseObjectCollectionComp();
	virtual ~CDataBaseObjectCollectionComp();

	virtual void Refresh();

	static inline std::pair<QString, QVariant> MakeSqlBindBalue(const QByteArray& propName, const IDataBaseChangeable* const object);
	static inline QVariantMap MakeSqlBindBaluesList(const IDataBaseChangeable* const object);

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
	const IDataBaseChangeable* const GetObjectPtrById(const QByteArray& id) const;
	Q_REQUIRED_RESULT virtual inline QSqlQuery ExecSelectSqlQuery(
				_In_ const IDataBaseChangeable* const object,
				_In_opt_ QVariantMap bindValues = {},
				_Outptr_opt_ QSqlError* sqlError = nullptr) const;
	virtual inline QSqlQuery ExecUpdateSqlQuery(
				_In_ const IDataBaseChangeable* const object,
				_In_ QVariantMap bindValues,
				_Outptr_opt_ QSqlError* sqlError = nullptr) const;
	Q_REQUIRED_RESULT virtual inline QSqlQuery ExecInsertSqlQuery(
				_In_ const IDataBaseChangeable* const object,
				_In_ QVariantMap bindValues,
				_Outptr_opt_ QSqlError* sqlError = nullptr) const;
	virtual inline QSqlQuery ExecDeleteSqlQuery(
				_In_ const IDataBaseChangeable* const object,
				_In_opt_ QVariantMap bindValues,
				_Outptr_opt_ QSqlError* sqlError = nullptr) const;

	// reimplemented (TLoggerCompWrap)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (CObjectCollectionBase)
	virtual istd::IChangeable* CreateObjectInstance(const QByteArray& typeId) const override;

private:
	QByteArray ExtractValueFromFileOrAttr(icomp::TAttributeMember<iattr::TAttribute<QByteArray>>* attr, icomp::TReferenceMember<ifile::IFileNameParam>* fileAttr);

protected:
	I_REF(IDatabaseEngine, m_dbEngine);
	I_ATTR(bool, m_updateOnDataBaseConnected);
};


} // namespace imtdb


