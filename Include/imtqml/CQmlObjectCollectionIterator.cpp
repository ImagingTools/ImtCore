// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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


