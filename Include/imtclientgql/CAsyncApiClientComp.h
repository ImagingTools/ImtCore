// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtclientgql/IAsyncGqlClient.h>
#include <imtclientgql/IClientProtocolEngine.h>


class QNetworkAccessManager;


namespace imtclientgql
{


/**
	Standard implementation of \c IAsyncGqlClient.

	Asynchronous counterpart of \c CApiClientComp: dispatches GraphQL
	queries / mutations to the server via a non-blocking POST on top of
	\c QNetworkAccessManager, reusing the same \c IClientProtocolEngine
	used by the synchronous client.

	Configuration is intentionally identical to \c CApiClientComp
	(\c ProtocolEngine reference + \c Timeout attribute) so this component
	is a drop-in replacement in partitura files when the async transport
	is desired.

	The future is completed on this component's QObject thread (the thread
	of the owned \c QNetworkAccessManager).
*/
class CAsyncApiClientComp:
			public ilog::CLoggerComponentBase,
			virtual public IAsyncGqlClient
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CAsyncApiClientComp);
		I_REGISTER_INTERFACE(IAsyncGqlClient);
		I_ASSIGN(m_protocolEngineCompPtr, "ProtocolEngine", "Protocol engine used for the communication with the server", true, "ProtocolEngine");
		I_ASSIGN(m_timeoutAttrPtr, "Timeout", "Request timeout in seconds (0 = no timeout)", true, 30.0);
	I_END_COMPONENT

	CAsyncApiClientComp();
	virtual ~CAsyncApiClientComp();

	// reimplemented (IAsyncGqlClient)
	virtual QFuture<GqlResult> SendRequest(
				GqlRequestPtr requestPtr,
				imtbase::IUrlParam* urlParamPtr = nullptr) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	void SendRequestInternal(
			std::shared_ptr<QPromise<GqlResult>> promisePtr,
			GqlRequestPtr requestPtr,
			const QNetworkRequest& networkRequest) const;

	I_REF(imtclientgql::IClientProtocolEngine, m_protocolEngineCompPtr);
	I_ATTR(double, m_timeoutAttrPtr);

	int m_timeout = 30000; // milliseconds
	QThread* m_thread = nullptr;
	QNetworkAccessManager* m_networkManager = nullptr;
};


} // namespace imtclientgql
