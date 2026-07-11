// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// Qt includes
#include <QtCore/QMap>

// ImtCore includes
#include <imtqml/IDataModelBridge.h>


namespace imtqml
{


/**
	\brief Demultiplexing in-process implementation of
	\c IDataModelBridge.

	\details
	\c CDataModelBridgeDemultiplexer does not serve any model on its
	own. Instead it owns N delegate bridges (also \c IDataModelBridge
	instances) connected via \c I_MULTIREF and routes every
	\c GetModel / \c SetModel call to the first delegate that reports
	\c IsSupported(modelId) for the requested \c modelId. This way one
	bridge serves N model controllers in the in-process scenario —
	per-model delegates simply implement \c IDataModelBridge,
	override \c IsSupported to claim their \c modelId(s) and are
	plugged into this demultiplexer through the
	\c ModelDelegates icomp slot.

	If no delegate accepts the \c modelId the request fails with a
	descriptive error message.

	\note Bridge is intentionally NOT exposed to QML — clients
	(\c CDataModelController) resolve it through icomp.
*/
class CDataModelBridgeDemultiplexer:
			public icomp::CComponentBase,
			virtual public IDataModelBridge
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDataModelBridgeDemultiplexer);
		I_REGISTER_INTERFACE(IDataModelBridge);
		I_ASSIGN_MULTI_0(m_modelDelegateCompPtr, "ModelDelegates",
				"Per-model delegate bridges resolved by modelId", false);
	I_END_COMPONENT;

	~CDataModelBridgeDemultiplexer() override;

	/**
		\brief Returns the most recently created instance of this
		bridge or \c nullptr if none exists.

		\details Mirrors the lookup pattern used by
		\c CDocumentServiceBridge::Instance() so a QML-facing controller
		(\c CDataModelController) can resolve the bridge without
		knowing about icomp. The instance is registered in
		\c OnComponentCreated() and unregistered in the destructor.
	*/
	static CDataModelBridgeDemultiplexer* Instance();

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

public:
	// reimplemented (IDataModelBridge)
	virtual bool IsSupported(const QString& modelId) const override;

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

private:
	IDataModelBridge* FindDelegate(const QString& modelId) const;

private:
	struct SubscriptionInfo
	{
		IDataModelBridge* delegatePtr = nullptr;
		int delegateSubscriptionId = 0;
	};

	I_MULTIREF(IDataModelBridge, m_modelDelegateCompPtr);

	QMap<int, SubscriptionInfo> m_subscriptions;
	int m_nextSubscriptionId = 1;

	static CDataModelBridgeDemultiplexer* s_instancePtr;
};


} // namespace imtqml
