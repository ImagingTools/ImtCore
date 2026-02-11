// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QtGlobal>

// ACF includes
#include <imod/CMultiModelDispatcherBase.h>


namespace imtbase
{


/**
	Helper for managing data change notifications from sub-models.
	In the context of an observer of some hierarchical data model it is usefull to have a mechanism for the reaction on changes of a sub-model.
	\code
	class IAddress: public istd::IChangeable
	{
	public:
		virtual QString GetCity() const = 0;
	};

	class IUser: public istd::IChangeable
	{
	public:
		virutal const QString GetName() const = 0; // Observed data model

		virutal const IAddress* GetAddress() const = 0; // Sub-model
	};

	class UserObserver: public imod::TSingleModelObserverBase<IUser>
	{
	public:
		// reimplemented (imod::IObserver)
		virtual bool OnModelAttached(imod::IModel* modelPtr, istd::IChangeable::ChangeSet& changeMask);
		{
			IUser* userPtr = GetObservedObject();

			m_addressObserver.RegisterObject(
						userPtr->GetAddress(),
						&UserObserver::OnAddressUpdated);
		}

	private:
		void OnAddressUpdated(const istd::IChangeable::ChangeSet& changeSet, const IAddress* addressPtr)
		{
			// Some reaction on the update of the sub-model (Address)
		}

	private:
		mutable imtbase::TModelUpdateBinder<IAddress, UserObserver> m_addressObserver;
	};
	\endcode
*/
template <typename ModelInterface, typename Parent>
class TModelUpdateBinder: protected imod::CMultiModelDispatcherBase
{
public:
	typedef void (Parent::*CallbackMethod)(const istd::IChangeable::ChangeSet& changeSet, const ModelInterface* objectPtr);
	typedef imod::CMultiModelDispatcherBase BaseClass;

	explicit TModelUpdateBinder(Parent& parent);

	bool RegisterObject(const ModelInterface* dataPtr, CallbackMethod callbackMethod, int modelId = 0);
	bool RegisterObject(ModelInterface* dataPtr, CallbackMethod callbackMethod, int modelId = 0);
	void UnregisterObject(int modelId);
	void UnregisterAllObjects();

	ModelInterface* GetObjectAt(int modelId) const;

	// reimplemented (imod::CMultiModelDispatcherBase)
	virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

private:
	Parent& m_parent;

	typedef QMap<int, CallbackMethod> CallbackMap;
	CallbackMap m_callbackMap;
};


template <typename ModelInterface, typename Parent>
TModelUpdateBinder<ModelInterface, Parent>::TModelUpdateBinder(Parent& parent)
	:m_parent(parent)
{
}


template <typename ModelInterface, typename Parent>
bool TModelUpdateBinder<ModelInterface, Parent>::RegisterObject(const ModelInterface* dataPtr, CallbackMethod callbackMethod, int modelId)
{
	return RegisterObject(const_cast<ModelInterface*>(dataPtr), callbackMethod, modelId);
}


template <typename ModelInterface, typename Parent>
bool TModelUpdateBinder<ModelInterface, Parent>::RegisterObject(ModelInterface* dataPtr, CallbackMethod callbackMethod, int modelId)
{
	auto modelPtr = dynamic_cast<imod::IModel*>(dataPtr);
	if (modelPtr != nullptr){
		m_callbackMap[modelId] = callbackMethod;
		if (BaseClass::RegisterModel(modelPtr, modelId)){
			return true;
		}

		m_callbackMap.remove(modelId);
	}

	return false;
}


template<typename ModelInterface, typename Parent>
void TModelUpdateBinder<ModelInterface, Parent>::UnregisterObject(int modelId)
{
	BaseClass::UnregisterModel(modelId);

	m_callbackMap.remove(modelId);
}


template<typename ModelInterface, typename Parent>
void TModelUpdateBinder<ModelInterface, Parent>::UnregisterAllObjects()
{
	BaseClass::UnregisterAllModels();

	m_callbackMap.clear();
}


template<typename ModelInterface, typename Parent>
ModelInterface* TModelUpdateBinder<ModelInterface, Parent>::GetObjectAt(int modelId) const
{
	return BaseClass::GetObjectAt<ModelInterface>(modelId);
}


// reimplemented (imod::CMultiModelDispatcherBase)

template <typename ModelInterface, typename Parent>
void TModelUpdateBinder<ModelInterface, Parent>::OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet)
{
	if (m_callbackMap.contains(modelId)){
		const CallbackMethod& method = m_callbackMap[modelId];

		(m_parent.*method)(changeSet, GetObjectAt(modelId));
	}
}


} // namespace imtbase

