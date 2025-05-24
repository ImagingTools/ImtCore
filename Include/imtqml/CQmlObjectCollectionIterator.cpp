#include <imtqml/CQmlObjectCollectionIterator.h>


namespace imtqml
{


// public methods

CQmlObjectCollectionIterator::CQmlObjectCollectionIterator(imtbase::IObjectCollectionIterator* iterator)
{
	m_objectCollectionIterator = iterator;
}


bool CQmlObjectCollectionIterator::next()
{
	if (m_objectCollectionIterator != nullptr){
		return m_objectCollectionIterator->Next();
	}

	return false;
}


bool CQmlObjectCollectionIterator::previous()
{
	if (m_objectCollectionIterator != nullptr){
		return m_objectCollectionIterator->Previous();
	}

	return false;
}


QString CQmlObjectCollectionIterator::getData(const QByteArray& fieldName)
{
	if (m_objectCollectionIterator != nullptr){
		return m_objectCollectionIterator->GetElementInfo(fieldName).toString();
	}

	return QString();
}


} // namespace imtqml


