#pragma once


// ACF includes
#include <idoc/CSerializedUndoManagerComp.h>

// ImtCore includes
#include <imtbase/ITransactionManager.h>


namespace imtbase
{


class CTransactionableUndoManagerComp:
			public idoc::CSerializedUndoManagerComp,
			virtual public ITransactionManager
{
public:
	typedef idoc::CSerializedUndoManagerComp BaseClass;

	I_BEGIN_COMPONENT(CTransactionableUndoManagerComp);
		I_REGISTER_INTERFACE(ITransactionManager);
	I_END_COMPONENT;

	CTransactionableUndoManagerComp();

	// reimplemented (ITransactionManager)
	virtual bool StartTransaction() override;
	virtual bool EndTransaction() override;

	// reimplemented (imod::IObserver)
	virtual void BeforeUpdate(imod::IModel* modelPtr) override;
	virtual void AfterUpdate(imod::IModel* modelPtr, const istd::IChangeable::ChangeSet& changeSet) override;

private:
	bool m_transactionInProgress;
	bool m_updateInProgress;
};


} // namespace imtbase


