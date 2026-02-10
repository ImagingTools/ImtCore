// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QTimer>
#include <QtCore/QMutex>
#include <QReadWriteLock>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <imod/IModel.h>
#include <imod/CSingleModelObserverBase.h>

// ImtCore includes
#include <imtcom/IConnectionStatusProvider.h>
#include <imtrest/ISender.h>
#include <imtrest/IRequestManager.h>
#include <imtrest/IRequestServlet.h>
#include <imtrest/IProtocolEngine.h>
#include <imtgql/CGqlRequest.h>
#include <imtclientgql/IGqlClient.h>
#include <imtclientgql/IGqlSubscriptionManager.h>


namespace imtclientgql
{


class CSubscriptionManagerComp:
			public QObject,
			public ilog::CLoggerComponentBase,
			public imod::CSingleModelObserverBase,
			virtual public IGqlSubscriptionManager,
			virtual public imtrest::IRequestServlet,
			virtual public IGqlClient
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSubscriptionManagerComp);
		I_REGISTER_INTERFACE(IGqlSubscriptionManager);
		I_REGISTER_INTERFACE(IGqlClient);
		I_REGISTER_INTERFACE(imtrest::IRequestServlet);
		I_ASSIGN(m_subscriptionSenderCompPtr, "SubscriptionSender", "Subscription sender", false, "SubscriptionSender");
		I_ASSIGN(m_requestManagerCompPtr, "RequestManager", "Request manager for sending a request", false, "RequestManager");
		I_ASSIGN(m_engineCompPtr, "ProtocolEngine", "Protocol engine for subscription", true, "ProtocolEngine");
		I_ASSIGN(m_connectionStatusProviderCompPtr, "WebLoginStatus", "Web login status", false, "WebLoginStatus");
		I_ASSIGN_TO(m_connectionStatusProviderModelCompPtr, m_connectionStatusProviderCompPtr, true);
	I_END_COMPONENT;

	// reimplemented (imtclientgql::IGqlSubscriptionManager)
	virtual QByteArray RegisterSubscription(const imtgql::IGqlRequest& subscriptionRequest, imtclientgql::IGqlSubscriptionClient * subscriptionClientPtr) override;
	virtual bool UnregisterSubscription(const QByteArray& subscriptionId) override;

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (imtrest::IRequestServlet)
	virtual bool IsCommandSupported(const QByteArray& commandId) const override;
	virtual imtrest::ConstResponsePtr ProcessRequest(const imtrest::IRequest& request, const QByteArray& subCommandId = QByteArray()) const override;

	// reimplemented (IGqlClient)
	virtual GqlResponsePtr SendRequest(GqlRequestPtr requestPtr, imtbase::IUrlParam* urlParamPtr = nullptr) const override;

protected:
	virtual void SubscriptionRegister(const imtgql::CGqlRequest& subscriptionRequest, const QByteArray& subscriptionId) const;
	virtual bool SendRequestInternal(const imtgql::IGqlRequest& request, imtrest::ConstRequestPtr& requestPtr) const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

Q_SIGNALS:
	void OnQueryDataReceived(int resultCode = 1) const;

private:
	virtual imtrest::ConstResponsePtr CreateErrorResponse(const QByteArray& errorMessage, const imtrest::IRequest& request) const;

private:
	I_REF(imtrest::ISender, m_subscriptionSenderCompPtr);
	I_REF(imtrest::IRequestManager, m_requestManagerCompPtr);
	I_REF(imtcom::IConnectionStatusProvider, m_connectionStatusProviderCompPtr);
	I_REF(imod::IModel, m_connectionStatusProviderModelCompPtr);
	I_REF(imtrest::IProtocolEngine, m_engineCompPtr);

	class SubscriptionHelper
	{
	public:
		imtgql::CGqlRequest m_request;
		QByteArray m_clientId;
		IGqlSubscriptionClient::SubscriptionStatus m_status;
		QList<IGqlSubscriptionClient*> m_clients;
	};

	class NetworkOperation
	{
	public:
		NetworkOperation() = delete;
		NetworkOperation(int timeout, const CSubscriptionManagerComp* parent);
		~NetworkOperation();

		QEventLoop connectionLoop;
		bool timerFlag;
		QTimer timer;
	};

	mutable QMap <QByteArray, SubscriptionHelper> m_registeredClients;
	mutable QMap<QString, QByteArray> m_queryDataMap;
	mutable QMutex m_registeredClientsMutex;
	mutable QReadWriteLock m_queryDataMapLock;
};


} // namespace imtclientgql


