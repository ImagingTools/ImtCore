#include "TestPublisher.h"

// Qt includes
#include <QtCore/QDateTime>
#include <QtCore/QJsonObject>


TestPublisher::TestPublisher(): QObject()
{
	connect(&m_dateTimer, &QTimer::timeout, this, &TestPublisher::slotTimer);
	connect(this, &TestPublisher::signalStartTimer, &m_dateTimer, QOverload<>::of(&QTimer::start), Qt::QueuedConnection);
	connect(this, &TestPublisher::signalStopTimer, &m_dateTimer, &QTimer::stop, Qt::QueuedConnection);
	m_dateTimer.setInterval(5000);
}


void TestPublisher::slotTimer()
{
	for (const QByteArray& subscriptionId: m_subscriptionIdList){
		QJsonObject jsonData;
		jsonData.insert("currentData", QDateTime::currentDateTime().toString());

		graphqlserver::PublishData(subscriptionId, jsonData);  //
	}
}


bool TestPublisher::RegisterSubscription(
			const QByteArray& subscriptionId,
			const graphqlserver::ResultKeys& resultKeys,
			QString& errorMessage)
{
	m_subscriptionIdList.append(subscriptionId);
	if (!m_dateTimer.isActive()){
		signalStartTimer();
	}

	return true;
}


bool TestPublisher::UnRegisterSubscription(const QByteArray& subscriptionId, QString &errorMessage)
{
	m_subscriptionIdList.removeAll(subscriptionId);
	if (m_subscriptionIdList.isEmpty()){
		signalStopTimer();
	}

	return true;
}


