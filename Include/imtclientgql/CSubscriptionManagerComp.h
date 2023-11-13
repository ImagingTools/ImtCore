#pragma once


// Qt includes
#include <QtWebSockets/QWebSocket>
#include <QtCore/QTimer>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <imod/IModel.h>
#include <imod/CSingleModelObserverBase.h>

// ImtCore includes
#include <imtauth/ILoginStatusProvider.h>
#include <imtrest/ISender.h>
#include <imtrest/IRequestManager.h>
#include <imtrest/IRequestServlet.h>
#include <imtrest/IProtocolEngine.h>
#include <imtgql/IGqlResponseHandler.h>
#include <imtclientgql/IGqlClient.h>
#include <imtclientgql/IGqlSubscriptionManager.h>


namespace imtclientgql
{


class CSubscriptionManagerComp:
			public QObject,
			public ilog::CLoggerComponentBase,
			public imod::CSingleModelObserverBase,
			virtual public imtclientgql::IGqlSubscriptionManager,
			virtual public imtrest::IRequestServlet,
			virtual public imtclientgql::IGqlClient
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSubscriptionManagerComp);
		I_REGISTER_INTERFACE(imtclientgql::IGqlSubscriptionManager);
		I_REGISTER_INTERFACE(imtclientgql::IGqlClient);
		I_REGISTER_INTERFACE(imtrest::IRequestServlet);
		I_ASSIGN(m_subsctiptionSenderCompPtr, "SubsctiptionSender", "Subsctiption sender", false, "SubsctiptionSender");
		I_ASSIGN(m_requestManagerCompPtr, "RequestManager", "Request manager for sending a request", false, "RequestManager");
		I_ASSIGN(m_engineCompPtr, "ProtocolEngine", "Protocol engine for subscription", true, "ProtocolEngine");
		I_ASSIGN(m_loginStatusCompPtr, "WebLoginStatus", "Web login status", false, "WebLoginStatus");
		I_ASSIGN_TO(m_webLoginStatusModelCompPtr, m_loginStatusCompPtr, true);
	I_END_COMPONENT;

	CSubscriptionManagerComp();

	// reimplemented (imtclientgql::IGqlSubscriptionManager)
	virtual QByteArray RegisterSubscription(const imtgql::CGqlRequest& subscriptionRequest, imtclientgql::IGqlSubscriptionClient * subscriptionClient) override;
	virtual bool UnregisterSubscription(const QByteArray& subscriptionId) override;

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (imtrest::IRequestServlet)
	virtual imtrest::ConstResponsePtr ProcessRequest(const imtrest::IRequest& request) const override;
	virtual QByteArray GetSupportedCommandId() const override;

	// reimplemented (IGqlClient)
	virtual bool SendRequest(const imtgql::IGqlRequest& request, imtgql::IGqlResponseHandler& responseHandler) const override;

protected:
	virtual void ServiceManagerRegister(const imtgql::CGqlRequest& subscriptionRequest, QByteArray subscriptionId) const;
	virtual bool SendRequestInternal(const imtgql::IGqlRequest& request, imtrest::ConstRequestPtr& requestPtr) const;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

Q_SIGNALS:
	void OnQueryDataReceived(int resultCode = 1) const;

private:
	virtual imtrest::ConstResponsePtr CreateErrorResponse(QByteArray errorMessage, const imtrest::IRequest& request) const;

private:
	I_REF(imtrest::ISender, m_subsctiptionSenderCompPtr);
	I_REF(imtrest::IRequestManager, m_requestManagerCompPtr);
	I_REF(imtauth::ILoginStatusProvider, m_loginStatusCompPtr);
	I_REF(imod::IModel, m_webLoginStatusModelCompPtr);
	I_REF(imtrest::IProtocolEngine, m_engineCompPtr);

	class SubscriptionHelper
	{
	public:
		imtgql::CGqlRequest m_request;
		QByteArray m_clientId;
		imtclientgql::IGqlSubscriptionClient::SubscriptionStatus m_status;
		QList<imtclientgql::IGqlSubscriptionClient*> m_clients;
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
	mutable imtauth::ILoginStatusProvider::LoginStatusFlags m_loginStatus;
	mutable QMap<QString, QByteArray> m_queryDataMap;
};


} // namespace imtclientgql


