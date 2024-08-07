#include <imtqml/CQmlObjectCollectionIterator.h>


// ImtCore includes
#include <imtdb/CSqlDatabaseObjectCollectionIterator.h>


namespace imtqml
{


// public methods

CQmlObjectCollectionIterator::CQmlObjectCollectionIterator(imtbase::IObjectCollectionIterator* iterator)
{
	m_objectCollectionIterator = iterator;
}


bool CQmlObjectCollectionIterator::next()
{
	if (m_objectCollectionIterator != nullptr) {
		return m_objectCollectionIterator->Next();
	}

	return false;
}


bool CQmlObjectCollectionIterator::previous()
{
	if (m_objectCollectionIterator != nullptr) {
		return m_objectCollectionIterator->Previous();
	}

	return false;
}


QString CQmlObjectCollectionIterator::getData(QString fieldName)
{
	imtdb::CSqlDatabaseObjectCollectionIterator* sqlIterator = dynamic_cast<imtdb::CSqlDatabaseObjectCollectionIterator*>(m_objectCollectionIterator);
	if (sqlIterator != nullptr) {
		QSqlRecord record = sqlIterator->GetRecord();
		if (!record.isEmpty() && record.contains(fieldName)) {
			QString fieldData = record.value(fieldName).toString();
			return fieldData;
		}
	}
	return QString();
}


} // namespace imtqml


