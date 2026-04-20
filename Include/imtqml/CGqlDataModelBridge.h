// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtclientgql/IGqlClient.h>
#include <imtqml/IDataModelBridge.h>


namespace imtqml
{


/**
	\brief GraphQL/SDL implementation of \c IDataModelBridge.

	\details
	An icomp component that owns an \c I_REF to an
	\c imtclientgql::IGqlClient and translates every
	\c IDataModelBridge \c GetModel / \c SetModel call into the
	corresponding SDL-generated GraphQL request. Operations are
	dispatched on a worker thread; results are marshalled to the GUI
	thread before invoking the supplied callback.

	\note Bridge is intentionally NOT exposed to QML — clients
	(\c CDataModelController) resolve it through icomp.
*/
class CGqlDataModelBridge:
			public icomp::CComponentBase,
			virtual public IDataModelBridge
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGqlDataModelBridge);
		I_REGISTER_INTERFACE(IDataModelBridge);
		I_ASSIGN(m_apiClientCompPtr, "ApiClient", "GraphQL API client", true, "ApiClient");
	I_END_COMPONENT;

	CGqlDataModelBridge();
	~CGqlDataModelBridge() override;

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

private:
	I_REF(imtclientgql::IGqlClient, m_apiClientCompPtr);
};


} // namespace imtqml
