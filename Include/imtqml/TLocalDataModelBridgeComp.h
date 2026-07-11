// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QVariantMap>

// ACF includes
#include <imod/IModel.h>
#include <imod/TSingleModelObserverBase.h>
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtqml/CDataModelBridgeBase.h>


namespace imtqml
{


/**
	\brief Desktop (in-process) implementation of \c IDataModelBridge
	that binds a local \c imod data model to the declarative
	QML ViewModel world.

	\details
	\c TLocalDataModelBridgeComp is the desktop counterpart of the
	GraphQL bridge: it serves a single \c modelId (configured via the
	inherited \c ModelId attribute) from a local data model referenced
	through the \c DataModel icomp slot.

	The \c imod::IObserver mechanics stay completely encapsulated in
	this bridge — QML editors and \c CDataModelController never see
	\c OnUpdate, change sets or update blockers:

	- \c GetModel serializes the observed object into a
	  \c QVariantMap via the abstract \c ReadObject and completes
	  synchronously;
	- \c SetModel writes a \c QVariantMap back into the object via
	  the abstract \c WriteObject inside a single
	  \c istd::CChangeNotifier transaction
	  (\c BeginChanges / \c EndChanges);
	- \c SubscribeModel registers a live-update callback; every model
	  change (observed via \c imod::TSingleModelObserverBase) pushes
	  a fresh \c ReadObject result to all subscribers.

	Concrete per-model bridges subclass this template and implement
	the two mapping methods \c ReadObject / \c WriteObject.

	\sa CDataModelBridgeBase, CDataModelBridgeDemultiplexer,
		CDataModelController, CObjectViewModel
*/
template <class ModelInterface>
class TLocalDataModelBridgeComp:
			public CDataModelBridgeBase,
			protected imod::TSingleModelObserverBase<ModelInterface>
{
public:
	typedef CDataModelBridgeBase BaseClass;
	typedef imod::TSingleModelObserverBase<ModelInterface> ObserverBaseClass;

	I_BEGIN_BASE_COMPONENT(TLocalDataModelBridgeComp);
		I_ASSIGN(m_dataModelCompPtr, "DataModel",
				"Local data model served by this bridge", true, "DataModel");
	I_END_COMPONENT;

	// reimplemented (IDataModelBridge)
	virtual void GetModel(
			const QString& modelId,
			const QVariantMap& parameters,
			GetModelCallback callback) override;

	virtual void SetModel(
			const QString& modelId,
			const QVariantMap& parameters,
			const QVariant& model,
			SetModelCallback callback) override;

	virtual int SubscribeModel(
			const QString& modelId,
			const QVariantMap& parameters,
			ModelUpdateCallback callback) override;

	virtual void UnsubscribeModel(int subscriptionId) override;

protected:
	/**
		\brief Map the data-model object to its declarative
		\c QVariantMap representation consumed by
		\c CObjectViewModel.
	*/
	virtual QVariantMap ReadObject(const ModelInterface& object) const = 0;

	/**
		\brief Write the declarative representation back into the
		data-model object. Called inside a single
		\c BeginChanges / \c EndChanges transaction.
	*/
	virtual bool WriteObject(const QVariantMap& values, ModelInterface& object) const = 0;

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	I_REF(imod::IModel, m_dataModelCompPtr);

	QMap<int, ModelUpdateCallback> m_subscribers;
	int m_nextSubscriptionId = 1;
};


// public methods

// reimplemented (IDataModelBridge)

template <class ModelInterface>
void TLocalDataModelBridgeComp<ModelInterface>::GetModel(
		const QString& modelId,
		const QVariantMap& parameters,
		GetModelCallback callback)
{
	Q_UNUSED(modelId);
	Q_UNUSED(parameters);

	if (!callback){
		return;
	}

	const ModelInterface* objectPtr = ObserverBaseClass::GetObservedObject();
	if (objectPtr == nullptr){
		callback(QVariant{}, QStringLiteral(
				"TLocalDataModelBridgeComp: no data model attached"));
		return;
	}

	callback(QVariant::fromValue(ReadObject(*objectPtr)), QString{});
}


template <class ModelInterface>
void TLocalDataModelBridgeComp<ModelInterface>::SetModel(
		const QString& modelId,
		const QVariantMap& parameters,
		const QVariant& model,
		SetModelCallback callback)
{
	Q_UNUSED(modelId);
	Q_UNUSED(parameters);

	ModelInterface* objectPtr = ObserverBaseClass::GetObservedObject();
	if (objectPtr == nullptr){
		if (callback){
			callback(QStringLiteral(
					"TLocalDataModelBridgeComp: no data model attached"));
		}
		return;
	}

	bool isWritten = false;
	{
		istd::CChangeNotifier changeNotifier(objectPtr);
		isWritten = WriteObject(model.toMap(), *objectPtr);
	}

	if (callback){
		callback(isWritten ?
				QString{} :
				QStringLiteral("TLocalDataModelBridgeComp: writing the model failed"));
	}
}


template <class ModelInterface>
int TLocalDataModelBridgeComp<ModelInterface>::SubscribeModel(
		const QString& modelId,
		const QVariantMap& parameters,
		ModelUpdateCallback callback)
{
	Q_UNUSED(modelId);
	Q_UNUSED(parameters);

	if (!callback){
		return 0;
	}

	int subscriptionId = m_nextSubscriptionId++;
	m_subscribers.insert(subscriptionId, callback);

	return subscriptionId;
}


template <class ModelInterface>
void TLocalDataModelBridgeComp<ModelInterface>::UnsubscribeModel(int subscriptionId)
{
	m_subscribers.remove(subscriptionId);
}


// protected methods

// reimplemented (imod::CSingleModelObserverBase)

template <class ModelInterface>
void TLocalDataModelBridgeComp<ModelInterface>::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	Q_UNUSED(changeSet);

	if (m_subscribers.isEmpty()){
		return;
	}

	const ModelInterface* objectPtr = ObserverBaseClass::GetObservedObject();
	if (objectPtr == nullptr){
		return;
	}

	QVariant model = QVariant::fromValue(ReadObject(*objectPtr));

	const auto subscribers = m_subscribers;
	for (const ModelUpdateCallback& subscriberCallback: subscribers){
		subscriberCallback(model);
	}
}


// reimplemented (icomp::CComponentBase)

template <class ModelInterface>
void TLocalDataModelBridgeComp<ModelInterface>::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_dataModelCompPtr.IsValid()){
		m_dataModelCompPtr->AttachObserver(this);
	}
}


template <class ModelInterface>
void TLocalDataModelBridgeComp<ModelInterface>::OnComponentDestroyed()
{
	ObserverBaseClass::EnsureModelDetached();

	BaseClass::OnComponentDestroyed();
}


} // namespace imtqml
