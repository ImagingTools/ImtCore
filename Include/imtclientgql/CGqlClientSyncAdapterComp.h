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
	Adapter that exposes the synchronous \c IGqlClient interface on top of
	an asynchronous \c IAsyncGqlClient.

	Lets existing high-level components written against \c IGqlClient
	(e.g. \c TClientRequestManagerCompWrap, \c CGqlObjectCollectionComp,
	\c CGqlHierarchicalStructureComp, \c CGqlRemoteRepresentationControllerCompBase,
	\c CRemoteGqlCollectionController) keep their synchronous semantics
	while the underlying transport runs asynchronously — the "Path A"
	integration option of the async communication plan.

	On each \c SendRequest call the adapter forwards the request to the
	configured \c AsyncClient, installs an internal response handler and
	blocks until the response arrives or the configured \c Timeout
	elapses. On timeout the in-flight request is cancelled before
	returning to ensure the handler has been invoked exactly once and
	no waiter is left dangling.
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

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_REF(imtclientgql::IAsyncGqlClient, m_asyncClientCompPtr);
	I_ATTR(double, m_timeoutAttrPtr);

	int m_timeout;
};


} // namespace imtclientgql

