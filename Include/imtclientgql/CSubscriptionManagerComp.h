#pragma once


// Qt includes
#include <QWebSocket>
#include <QtCore/QTimer>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <imod/IModel.h>
#include <imod/CSingleModelObserverBase.h>

// ImtCore includes
#include <imtclientgql/IGqlSubscriptionManager.h>
#include <imtauth/ILoginStatusProvider.h>
#include <imtrest/ISender.h>
#include <imtrest/IRequestServlet.h>
#include <imtrest/IProtocolEngine.h>



namespace imtclientgql
{


class CSubscriptionManagerComp:
			public QObject,
			public ilog::CLoggerComponentBase,
			public imod::CSingleModelObserverBase,
			virtual public imtclientgql::IGqlSubscriptionManager,
			virtual public imtrest::IRequestServlet
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSubscriptionManagerComp);
		I_REGISTER_INTERFACE(imtclientgql::IGqlSubscriptionManager);
		I_ASSIGN(m_subsctiptionSenderCompPtr, "SubsctiptionSender", "Subsctiption sender", true, "SubsctiptionSender");
		I_ASSIGN(m_engineCompPtr, "ProtocolEngine", "Protocol engine for subscription", true, "ProtocolEngine");
		I_ASSIGN(m_loginStatusCompPtr, "WebLoginStatus", "Web login status", true, "WebLoginStatus");
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

protected:
	virtual void ServiceManagerRegister(const imtgql::CGqlRequest& subscriptionRequest, QByteArray subscriptionId) const;

//	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
//	virtual void OnComponentDestroyed() override;


//private Q_SLOTS:
//	void OnConnectedTimer();

private:
//	virtual void Connect();
	virtual imtrest::ConstResponsePtr CreateErrorResponse(QByteArray errorMessage, const imtrest::IRequest& request) const;

private:
	I_REF(imtrest::ISender, m_subsctiptionSenderCompPtr);
	I_REF(imtauth::ILoginStatusProvider, m_loginStatusCompPtr);
	I_REF(imod::IModel, m_webLoginStatusModelCompPtr);
	I_REF(imtrest::IProtocolEngine, m_engineCompPtr);
	I_REF(imtrest::IRequestServlet, m_requestHandlerCompPtr);

	class SubscriptionHelper
	{
	public:
		imtgql::CGqlRequest m_request;
		imtclientgql::IGqlSubscriptionClient::SubscriptionStatus m_status;
		QList<imtclientgql::IGqlSubscriptionClient*> m_clients;
	};

	mutable QMap <QByteArray, SubscriptionHelper> m_registeredClients;
	mutable imtauth::ILoginStatusProvider::LoginStatusFlags m_loginStatus;
};


} // namespace imtclientgql


