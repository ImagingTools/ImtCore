#pragma once


// ImtCore includes
#include <graphqlserver/graphqlserver.h>
#include <graphqlserver/CExternGraphQl.h>
#include <imtservergql/CGqlSubscriberControllerCompBase.h>


namespace graphqlserver
{


class CExternGraphQlPublisherComp:
			public imtservergql::CGqlSubscriberControllerCompBase,
			public CExternGraphQl
{
public:
	typedef imtservergql::CGqlSubscriberControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CExternGraphQlPublisherComp);
		// I_REGISTER_INTERFACE(graphqlserver::IGqlPublisher);
	I_END_COMPONENT;

	bool RegisterGqlPublisher(const QByteArray& commandId, IGqlPublisher& gqlPublisher, QString& errorMessage);
	bool SendSubscription(const QByteArray& subscriptionId, const QJsonObject& subscriptionData);
protected:
	// reimplemented (imtgql::IGqlSubscriberController)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;
	virtual bool RegisterSubscription(
		const QByteArray& subscriptionId,
		const imtgql::CGqlRequest& gqlRequest,
		const imtrest::IRequest& networkRequest,
		QString& errorMessage) override;
	virtual bool UnRegisterSubscription(const QByteArray& subscriptionId) override;

private:
	QMap<QByteArray, graphqlserver::IGqlPublisher*> m_gqlPublishers; // <CommandId, IGqlPublisherPtr>
	QMap<QByteArray, graphqlserver::IGqlPublisher*> m_gqlSubscriptions; // <SubscriptionId, IGqlPublisherPtr>
};


} // namespace graphqlserver


