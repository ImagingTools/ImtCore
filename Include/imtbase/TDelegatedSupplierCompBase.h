#pragma once


// ACF includes
#include <imod/IModel.h>
#include <imod/CModelUpdateBridge.h>
#include <iinsp/TSupplierCompWrap.h>


namespace imtbase
{


/**
	Generic implementation of a supplier that delegates his calls to an underlaying supplier implementation or to an existing data object.
*/
template <class ProviderInterface, class ObjectInterface>
class TDelegatedSupplierCompBase:
			public ilog::CLoggerComponentBase,
			virtual public ProviderInterface,
			virtual public iinsp::ISupplier
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;
	typedef typename ProviderInterface ProviderInterfaceType;
	typedef typename ObjectInterface ObjectInterfaceType;

	I_BEGIN_BASE_COMPONENT(TDelegatedSupplierCompBase);
		I_REGISTER_INTERFACE(iinsp::ISupplier);
		I_REGISTER_INTERFACE(ProviderInterfaceType);
		I_TASSIGN(m_objectCompPtr, "Object", "Object used if no slave supplier is provided", false, "Object");
		I_ASSIGN_TO(m_objectModelCompPtr, m_objectCompPtr, false);
		I_TASSIGN(m_objectProviderCompPtr, "SlaveSupplier", "Slave supplier where all calls will be delegated", false, "SlaveSupplier");
		I_ASSIGN_TO(m_objectSupplierCompPtr, m_objectProviderCompPtr, false);
		I_ASSIGN_TO(m_objectProviderModelCompPtr, m_objectProviderCompPtr, false);
	I_END_COMPONENT;

	TDelegatedSupplierCompBase();

	// reimplemented (iinsp::ISupplier)
	virtual int GetWorkStatus() const override;
	virtual imod::IModel* GetWorkStatusModel() const override;
	virtual void InvalidateSupplier() override;
	virtual void EnsureWorkInitialized() override;
	virtual void EnsureWorkFinished() override;
	virtual void ClearWorkResults() override;
	virtual const ilog::IMessageContainer* GetWorkMessages(int containerType) const override;
	virtual iprm::IParamsSet* GetModelParametersSet() const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

protected:
	I_TREF(ObjectInterfaceType, m_objectCompPtr);
	I_REF(imod::IModel, m_objectModelCompPtr);

	I_TREF(ProviderInterface, m_objectProviderCompPtr);
	I_REF(iinsp::ISupplier, m_objectSupplierCompPtr);
	I_REF(imod::IModel, m_objectProviderModelCompPtr);

	imod::CModelUpdateBridge m_updateBridge;
};


template <class ProviderInterface, class ObjectInterface>
TDelegatedSupplierCompBase<ProviderInterface, ObjectInterface>::TDelegatedSupplierCompBase()
	:m_updateBridge(this, imod::CModelUpdateBridge::UF_SOURCE)
{
}


// reimplemented (iinsp::ISupplier)

template <class ProviderInterface, class ObjectInterface>
int TDelegatedSupplierCompBase<ProviderInterface, ObjectInterface>::GetWorkStatus() const
{
	if (m_objectSupplierCompPtr.IsValid()){
		return m_objectSupplierCompPtr->GetWorkStatus();
	}

	if (m_objectCompPtr.IsValid()){
		return WS_OK;
	}

	return WS_FAILED;
}


template <class ProviderInterface, class ObjectInterface>
imod::IModel* TDelegatedSupplierCompBase<ProviderInterface, ObjectInterface>::GetWorkStatusModel() const
{
	if (m_objectSupplierCompPtr.IsValid()){
		return m_objectSupplierCompPtr->GetWorkStatusModel();
	}

	return NULL;
}


template <class ProviderInterface, class ObjectInterface>
void TDelegatedSupplierCompBase<ProviderInterface, ObjectInterface>::InvalidateSupplier()
{
	if (m_objectSupplierCompPtr.IsValid()){
		m_objectSupplierCompPtr->InvalidateSupplier();
	}
}


template <class ProviderInterface, class ObjectInterface>
void TDelegatedSupplierCompBase<ProviderInterface, ObjectInterface>::EnsureWorkInitialized()
{
	if (m_objectSupplierCompPtr.IsValid()){
		m_objectSupplierCompPtr->EnsureWorkInitialized();
	}
}


template <class ProviderInterface, class ObjectInterface>
void TDelegatedSupplierCompBase<ProviderInterface, ObjectInterface>::EnsureWorkFinished()
{
	if (m_objectSupplierCompPtr.IsValid()){
		m_objectSupplierCompPtr->EnsureWorkFinished();
	}
}


template <class ProviderInterface, class ObjectInterface>
void TDelegatedSupplierCompBase<ProviderInterface, ObjectInterface>::ClearWorkResults()
{
	if (m_objectSupplierCompPtr.IsValid()){
		m_objectSupplierCompPtr->ClearWorkResults();
	}
}


template <class ProviderInterface, class ObjectInterface>
const ilog::IMessageContainer* TDelegatedSupplierCompBase<ProviderInterface, ObjectInterface>::GetWorkMessages(int containerType) const
{
	if (m_objectSupplierCompPtr.IsValid()){
		return m_objectSupplierCompPtr->GetWorkMessages(containerType);
	}

	return NULL;
}


template <class ProviderInterface, class ObjectInterface>
iprm::IParamsSet* TDelegatedSupplierCompBase<ProviderInterface, ObjectInterface>::GetModelParametersSet() const
{
	if (m_objectSupplierCompPtr.IsValid()){
		return m_objectSupplierCompPtr->GetModelParametersSet();
	}

	return NULL;
}


// protected methods

// reimplemented (icomp::CComponentBase)

template <class ProviderInterface, class ObjectInterface>
void TDelegatedSupplierCompBase<ProviderInterface, ObjectInterface>::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_objectProviderModelCompPtr.IsValid()){
		m_objectProviderModelCompPtr->AttachObserver(&m_updateBridge);
	}
	else{
		if (m_objectModelCompPtr.IsValid()){
			m_objectModelCompPtr->AttachObserver(&m_updateBridge);
		}
	}
}


template <class ProviderInterface, class ObjectInterface>
void TDelegatedSupplierCompBase<ProviderInterface, ObjectInterface>::OnComponentDestroyed()
{
	m_updateBridge.EnsureModelsDetached();

	BaseClass::OnComponentDestroyed();
}


} // namespace imtbase


