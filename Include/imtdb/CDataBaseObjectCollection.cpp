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
		CDataBaseObjectCollection::CObjectCollectionBase::InsertNewObject("","","",newObj, "");
	}
}

QSqlQuery CDataBaseObjectCollection::ExecSelectSqlQuery(QVariantMap bindValues, QSqlError* sqlError) const
{
	return m_dbEngine->ExecSqlQuery(*m_selectSqlQueryString, bindValues, sqlError);
}

QSqlQuery CDataBaseObjectCollection::ExecUpdateSqlQuery(QVariantMap bindValues, QSqlError* sqlError) const
{
	return m_dbEngine->ExecSqlQuery(*m_updateSqlQueryString, bindValues, sqlError);
}

QSqlQuery CDataBaseObjectCollection::ExecInsertSqlQuery(QVariantMap bindValues, QSqlError* sqlError) const
{
	return m_dbEngine->ExecSqlQuery(*m_insertSqlQueryString, bindValues, sqlError);
}

QSqlQuery CDataBaseObjectCollection::ExecDeleteSqlQuery(QVariantMap bindValues, QSqlError* sqlError) const
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



} // namespace imod
