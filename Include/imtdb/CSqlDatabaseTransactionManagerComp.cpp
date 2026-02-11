// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CSqlDatabaseTransactionManagerComp.h>


namespace imtdb
{


// public methods

// reimplemented (imtbase::ITransactionManager)

bool CSqlDatabaseTransactionManagerComp::StartTransaction()
{
	if (!IsCollectionsInOneDatabase() || !m_databaseEngineCompPtr.IsValid()){
		return false;
	}

	if (m_databaseEngineCompPtr->BeginTransaction()){
		for (int i = 0; i < m_collectionCompPtr.GetCount(); i++){
			if (m_collectionCompPtr[i] != nullptr){
				m_collectionCompPtr[i]->SetInternalTransactionsEnabled(false);
			}
		}

		return true;
	}

	return false;
}


bool CSqlDatabaseTransactionManagerComp::CancelTransaction()
{
	if (!IsCollectionsInOneDatabase() || !m_databaseEngineCompPtr.IsValid()){
		return false;
	}

	for (int i = 0; i < m_collectionCompPtr.GetCount(); i++){
		if (m_collectionCompPtr[i] != nullptr){
			m_collectionCompPtr[i]->SetInternalTransactionsEnabled(true);
		}
	}

	return m_databaseEngineCompPtr->CancelTransaction();
}


bool CSqlDatabaseTransactionManagerComp::EndTransaction()
{
	if (!IsCollectionsInOneDatabase() || !m_databaseEngineCompPtr.IsValid()){
		return false;
	}

	for (int i = 0; i < m_collectionCompPtr.GetCount(); i++){
		if (m_collectionCompPtr[i] != nullptr){
			m_collectionCompPtr[i]->SetInternalTransactionsEnabled(true);
		}
	}

	return m_databaseEngineCompPtr->FinishTransaction();
}


// private methods

bool CSqlDatabaseTransactionManagerComp::IsCollectionsInOneDatabase() const
{
	QByteArray databaseId;

	for (int i = 0; i < m_collectionCompPtr.GetCount(); i++){
		if (m_collectionCompPtr[i] != nullptr){
			if (i == 0){
				databaseId = m_collectionCompPtr[i]->GetDatabaseId();
			}
			else{
				if (databaseId != m_collectionCompPtr[i]->GetDatabaseId()){
					return false;
				}
			}
		}
	}

	return true;
}


} // namespace imtdb


