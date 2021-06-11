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

}

istd::IChangeable* CDataBaseObjectCollection::CreateObjectInstance(const QByteArray& typeId) const
{
	return nullptr;
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

std::pair<QString, QVariant> CDataBaseObjectCollection::MakeSqlBindBalue(const QByteArray& propName, const IDataBaseChangeable* const object)
{
	std::pair<QByteArray, QVariant> retval = std::make_pair(QByteArray(), QVariant());
	if(object != nullptr)
	{
		retval = std::make_pair(QByteArray(":") + propName, object->GetProperty(propName));
	}
	return retval;
}
QVariantMap CDataBaseObjectCollection::MakeSqlBindBaluesList(const IDataBaseChangeable* const object)
{
	QVariantMap retval;
	QByteArrayList props = object->GetProperties();
	for(const auto& prop: ::qAsConst(props))
	{
		auto val = CDataBaseObjectCollection::MakeSqlBindBalue(prop, object);
		retval.insert(val.first, val.second);
	}
	return retval;
}


} // namespace imod
