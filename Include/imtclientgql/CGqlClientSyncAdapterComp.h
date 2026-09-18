// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtclientgql/IGqlClient.h>
#include <imtclientgql/IAsyncGqlClient.h>


namespace imtclientgql
{


/**
	Adapter: synchronous \c IGqlClient on top of \c IAsyncGqlClient.

	Keeps \c CSubscriptionManagerComp / \c CAsyncApiClientComp free of sync
	Wait logic. Wire this when a consumer needs \c IGqlClient (e.g.
	\c TClientRequestManagerCompWrap \c ApiClient) while the transport is async
	(\c WebSocketServerFramework exports this as \c IGqlClient).

	Each \c SendRequest forwards to \c AsyncClient and blocks until completion
	or \c Timeout (then cancels). Prefer \c IAsyncGqlClient on WebSocket worker
	threads to avoid nested event loops.
*/
class CGqlClientSyncAdapterComp:
			public ilog::CLoggerComponentBase,
			virtual public IGqlClient
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGqlClientSyncAdapterComp);
		I_REGISTER_INTERFACE(IGqlClient);
		I_ASSIGN(m_asyncClientCompPtr, "AsyncClient", "Asynchronous GraphQL client wrapped by this adapter", true, "AsyncClient");
		I_ASSIGN(m_timeoutAttrPtr, "Timeout", "Request timeout in seconds (0 = no timeout)", true, 30.0);
	I_END_COMPONENT

	CGqlClientSyncAdapterComp();

	// reimplemented (IGqlClient)
	virtual GqlResponsePtr SendRequest(GqlRequestPtr requestPtr, imtbase::IUrlParam* urlParamPtr = nullptr) const override;
	virtual bool SendRequestNoWait(GqlRequestPtr requestPtr, imtbase::IUrlParam* urlParamPtr = nullptr) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_REF(imtclientgql::IAsyncGqlClient, m_asyncClientCompPtr);
	I_ATTR(double, m_timeoutAttrPtr);

	int m_timeout;
};


} // namespace imtclientgql

