// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrepo/CFileObjectCollectionIterator.h>


// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtrepo
{


// public methods

	CFileObjectCollectionIterator::CFileObjectCollectionIterator(
		IFileObjectCollection* fileObjectCollectionPtr)
	:m_currentIndex(-1)
{
	
	m_fileObjectCollectionPtr = fileObjectCollectionPtr;
}


// reimplemented (imtbase::IObjectCollectionIterator)

bool CFileObjectCollectionIterator::Next() const
{
	int recordsSize = m_fileObjectCollectionPtr->GetElementsCount();
	if (m_currentIndex < recordsSize - 1){
		m_currentIndex++;

		return true;
	}

	return false;
}


bool CFileObjectCollectionIterator::Previous() const
{
	if (m_currentIndex > 0){
		m_currentIndex--;

		return true;
	}

	return false;
}


QByteArray CFileObjectCollectionIterator::GetObjectId() const
{
	return m_fileObjectCollectionPtr->GetElementIds()[m_currentIndex];

}


QByteArray CFileObjectCollectionIterator::GetObjectTypeId() const
{
	return m_fileObjectCollectionPtr->GetObjectTypeId(GetObjectId());
}


bool CFileObjectCollectionIterator::GetObjectData(imtbase::IObjectCollection::DataPtr& dataPtr) const
{
	return m_fileObjectCollectionPtr->GetObjectData(GetObjectId(), dataPtr);
}


idoc::MetaInfoPtr CFileObjectCollectionIterator::GetDataMetaInfo() const
{
	return m_fileObjectCollectionPtr->GetDataMetaInfo(GetObjectId());
}


idoc::MetaInfoPtr CFileObjectCollectionIterator::GetCollectionMetaInfo() const
{

	return m_fileObjectCollectionPtr->GetElementMetaInfo(GetObjectId());
}


QVariant CFileObjectCollectionIterator::GetElementInfo(int infoType) const
{
	return m_fileObjectCollectionPtr->GetElementInfo(GetObjectId(), infoType);
}


QVariant CFileObjectCollectionIterator::GetElementInfo(QByteArray /*infoId*/) const
{
	return QVariant();
}


} // namespace imtdb


