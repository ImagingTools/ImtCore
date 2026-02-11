// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdoc/CCollectionDocumentManagerComp.h>


namespace imtdoc
{


// protected methods

// reimplemented (imtdoc::CCollectionDocumentManager)

imtbase::IObjectCollection* CCollectionDocumentManagerComp::GetCollection() const
{
	if (m_collectionCompPtr.IsValid()){
		return m_collectionCompPtr.GetPtr();
	}

	return nullptr;
}


istd::IChangeableSharedPtr CCollectionDocumentManagerComp::CreateObject(const QByteArray& typeId) const
{
	int index = GetObjectFactoryIndex(typeId);
	if (index >= 0){
		return m_objectFactListCompPtr.CreateInstance(index);
	}

	Q_ASSERT_X(false, "CCollectionDocumentManagerComp::CreateObject", qPrintable(QString("Factory not found for the type: '%1'").arg(qPrintable(typeId))));

	return nullptr;
}


idoc::IUndoManagerSharedPtr CCollectionDocumentManagerComp::CreateUndoManager() const
{
	return m_undoManagerFactPtr.CreateInstance();
}


QList<imtdoc::IDocumentManagerEventHandler*> CCollectionDocumentManagerComp::GetDocumentManagerEventHandlers() const
{
	QList<imtdoc::IDocumentManagerEventHandler*> retVal;

	if (m_handlerCompPtr.IsValid()){
		for (int i = 0; i < m_handlerCompPtr.GetCount(); i++){
			if (m_handlerCompPtr[i] != nullptr){
				retVal.append(m_handlerCompPtr[i]);
			}
		}
	}

	return retVal;
}


// private methods

int CCollectionDocumentManagerComp::GetObjectFactoryIndex(const QByteArray& typeId) const
{
	int count = qMin(m_objectTypeIdsAttrPtr.GetCount(), m_objectFactListCompPtr.GetCount());
	for (int i = 0; i < count; i++){
		if (m_objectTypeIdsAttrPtr[i] == typeId){
			return i;
		}
	}

	return -1;
}


} // namespace imtdoc


