#include <imtdb/CDataBaseObjectCollection.h>


namespace imtdb
{

CDataBaseObjectCollection::CDataBaseObjectCollection():imtbase::CObjectCollectionBase()
{
}


CDataBaseObjectCollection::~CDataBaseObjectCollection()
{
}

void CDataBaseObjectCollection::Refresh()
{
	QSqlQuery sqlQuery= this->ExecSelectSqlQuery();
	while (sqlQuery.next()) {
		auto newObj = this->CreateObjectFromSqlRecord(sqlQuery.record());
		CDataBaseObjectCollection::CObjectCollectionBase::InsertNewObject("","","",newObj, newObj->GetID());
	}
}

QSqlQuery CDataBaseObjectCollection::ExecSelectSqlQuery(const QVariantMap bindValues, QSqlError* sqlError) const
{
	return m_dbEngine->ExecSqlQuery(*m_selectSqlQueryString, bindValues, sqlError);
}

QSqlQuery CDataBaseObjectCollection::ExecUpdateSqlQuery(const QVariantMap bindValues, QSqlError* sqlError) const
{
	return m_dbEngine->ExecSqlQuery(*m_updateSqlQueryString, bindValues, sqlError);
}

QSqlQuery CDataBaseObjectCollection::ExecInsertSqlQuery(const QVariantMap bindValues, QSqlError* sqlError) const
{
	return m_dbEngine->ExecSqlQuery(*m_insertSqlQueryString, bindValues, sqlError);
}

QSqlQuery CDataBaseObjectCollection::ExecDeleteSqlQuery(const QVariantMap bindValues, QSqlError* sqlError) const
{
	return m_dbEngine->ExecSqlQuery(*m_deleteSqlQueryString, bindValues, sqlError);
}

void CDataBaseObjectCollection::SetObjectName(const QByteArray& objectId, const QString& objectName)
{
	CObjectCollectionBase::SetObjectName(objectId, objectName);
}

void CDataBaseObjectCollection::SetObjectDescription(const QByteArray& objectId, const QString& objectDescription)
{
	CObjectCollectionBase::SetObjectDescription(objectId, objectDescription);
}

void CDataBaseObjectCollection::SetObjectEnabled(const QByteArray& objectId, bool isEnabled)
{
	CObjectCollectionBase::SetObjectEnabled(objectId, isEnabled);
}

const IDataBaseChangeable* const CDataBaseObjectCollection::GetObjectPtrById(const QByteArray& id) const
{
	const IDataBaseChangeable* retval = nullptr;
	retval = dynamic_cast<const IDataBaseChangeable*>(CObjectCollectionBase::GetObjectPtr(id));
	return retval;
}

const IDataBaseChangeable* CDataBaseObjectCollection::CreateObjectFromSqlRecord(const QSqlRecord& record) const
{
	Q_ASSERT_X(0, Q_FUNC_INFO, "Unreimplemented method call");
	return nullptr;
}

QByteArray CDataBaseObjectCollection::InsertNewObject(const QByteArray& typeId, const QString& name, const QString& description, const istd::IChangeable* defaultValuePtr, const QByteArray& preferredId)
{
	Q_ASSERT_X(0, Q_FUNC_INFO, "Unreimplemented method call");
	return nullptr;
}

bool CDataBaseObjectCollection::RemoveObject(const QByteArray& objectId)
{
	Q_ASSERT_X(0, Q_FUNC_INFO, "Unreimplemented method call");
	return false;
}

bool CDataBaseObjectCollection::SetObjectData(const QByteArray& objectId, const istd::IChangeable& object, CompatibilityMode mode)
{
	Q_ASSERT_X(0, Q_FUNC_INFO, "Unreimplemented method call");
	return false;
}

istd::IChangeable* CDataBaseObjectCollection::CreateObjectInstance(const QByteArray& typeId) const
{
	Q_ASSERT_X(0, Q_FUNC_INFO, "Unreimplemented method call");
	return nullptr;
}




} // namespace imod
