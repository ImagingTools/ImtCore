// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdb/CSqlDatabaseTransactionManagerComp.h>
#include <imtdb/IDependentMetaInfoNotifier.h>


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
				if (auto* notifier = dynamic_cast<IDependentMetaInfoNotifier*>(m_collectionCompPtr[i])) {
					notifier->BeginDeferredNotifications();
				}
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

	if (!m_databaseEngineCompPtr->CancelTransaction()) {
		return false;
	}
	for (int i = 0; i < m_collectionCompPtr.GetCount(); i++){
		if (m_collectionCompPtr[i] != nullptr){
			m_collectionCompPtr[i]->SetInternalTransactionsEnabled(true);
			if (auto* notifier = dynamic_cast<IDependentMetaInfoNotifier*>(m_collectionCompPtr[i])) {
				notifier->CompleteDeferredNotifications(false);
			}
		}
	}

	return true;
}


bool CSqlDatabaseTransactionManagerComp::EndTransaction()
{
	if (!IsCollectionsInOneDatabase() || !m_databaseEngineCompPtr.IsValid()){
		return false;
	}
	for (int index = 0; index < m_collectionCompPtr.GetCount(); ++index) {
		if (m_collectionCompPtr[index] != nullptr && m_collectionCompPtr[index]->IsTransactionRollbackOnly()) {
			CancelTransaction();
			return false;
		}
	}

	if (!m_databaseEngineCompPtr->FinishTransaction()) {
		return false;
	}
	for (int i = 0; i < m_collectionCompPtr.GetCount(); i++){
		if (m_collectionCompPtr[i] != nullptr){
			m_collectionCompPtr[i]->SetInternalTransactionsEnabled(true);
		}
	}

	for (int i = 0; i < m_collectionCompPtr.GetCount(); i++) {
		if (auto* notifier = dynamic_cast<IDependentMetaInfoNotifier*>(m_collectionCompPtr[i])) {
			notifier->CompleteDeferredNotifications(true);
		}
	}
	return true;
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


