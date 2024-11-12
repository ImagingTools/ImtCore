#include "TestPublisher.h"

// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QJsonObject>


TestPublisher::TestPublisher()
{
	connect(&m_dateTimer, &QTimer::timeout, this, &TestPublisher::slotTimer);
}


void TestPublisher::slotTimer()
{
	for (const QByteArray& subscriptionId: m_subscriptionIdList){
		QJsonObject jsonData;
		jsonData.insert("currentData", QDateTime::currentDateTime().toString());

		graphqlserver::SendSubscription(subscriptionId, jsonData);
	}
}


bool TestPublisher::IsRequestSupported(const graphqlserver::IGqlRequest& gqlRequest) const
{
	return true;
}


bool TestPublisher::RegisterSubscription(
			const QByteArray& subscriptionId,
			const graphqlserver::IGqlRequest& gqlRequest,
			QString& errorMessage)
{
	m_subscriptionIdList.append(subscriptionId);
	if (!m_dateTimer.isActive()){
		m_dateTimer.start();
	}

	return true;
}


bool TestPublisher::UnRegisterSubscription(const QByteArray& subscriptionId, QString &errorMessage)
{
	m_subscriptionIdList.removeAll(subscriptionId);
	if (m_subscriptionIdList.isEmpty()){
		m_dateTimer.stop();
	}

	return true;
}


