#include <imtbase/CTransactionableUndoManagerComp.h>


namespace imtbase
{


// public methods

CTransactionableUndoManagerComp::CTransactionableUndoManagerComp()
	:m_transactionInProgress(false),
	m_updateInProgress(false)
{
}


// reimplemented (ITransactionManager)

bool CTransactionableUndoManagerComp::StartTransaction()
{
	if (!m_updateInProgress && !m_transactionInProgress) {
		m_transactionInProgress = true;

		BaseClass::BeforeUpdate(GetObservedModel());

		return true;
	}

	return false;
}


bool CTransactionableUndoManagerComp::EndTransaction()
{
	if (!m_updateInProgress && m_transactionInProgress) {
		BaseClass::AfterUpdate(GetObservedModel(), istd::IChangeable::GetAnyChange());

		m_transactionInProgress = false;

		return true;
	}

	return false;
}


// reimplemented (imod::IObserver)

void CTransactionableUndoManagerComp::BeforeUpdate(imod::IModel* modelPtr)
{
	m_updateInProgress = true;

	if (!m_transactionInProgress) {
		BaseClass::BeforeUpdate(modelPtr);
	}
}


void CTransactionableUndoManagerComp::AfterUpdate(imod::IModel* modelPtr, const istd::IChangeable::ChangeSet& changeSet)
{
	if (!m_transactionInProgress) {
		BaseClass::AfterUpdate(modelPtr, changeSet);
	}

	m_updateInProgress = false;
}


} // namespace imtbase


