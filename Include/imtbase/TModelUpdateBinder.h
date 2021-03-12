#pragma once


// ACF includes
#include <imod/CMultiModelDispatcherBase.h>


namespace imtbase
{



template <typename ModelInterface, typename Parent>
class TModelUpdateBinder: protected imod::CMultiModelDispatcherBase
{
public:
	typedef typename void (Parent::*CallbackMethod)(const istd::IChangeable::ChangeSet& changeSet, const ModelInterface* objectPtr);
	typedef imod::CMultiModelDispatcherBase BaseClass;

	TModelUpdateBinder(Parent& parent);

	bool RegisterObject(const istd::IChangeable* dataPtr, CallbackMethod callbackMethod, int modelId = 0);
	bool RegisterObject(istd::IChangeable* dataPtr, CallbackMethod callbackMethod, int modelId = 0);
	void UnregisterObject(int modelId);
	void UnregisterAllObjects();

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
bool TModelUpdateBinder<ModelInterface, Parent>::RegisterObject(const istd::IChangeable* dataPtr, CallbackMethod callbackMethod, int modelId)
{
	return RegisterObject(const_cast<istd::IChangeable*>(dataPtr), callbackMethod, modelId);
}


template <typename ModelInterface, typename Parent>
bool TModelUpdateBinder<ModelInterface, Parent>::RegisterObject(istd::IChangeable* dataPtr, CallbackMethod callbackMethod, int modelId)
{
	imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(dataPtr);
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


// reimplemented (imod::CMultiModelDispatcherBase)

template <typename ModelInterface, typename Parent>
void TModelUpdateBinder<ModelInterface, Parent>::OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet)
{
	if (m_callbackMap.contains(modelId)){
		const CallbackMethod& method = m_callbackMap[modelId];

		(m_parent.*method)(changeSet, GetObjectAt<ModelInterface>(modelId));
	}
}


} // namespace imtbase


