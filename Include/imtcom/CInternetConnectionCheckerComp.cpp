#include <imtcom/CInternetConnectionCheckerComp.h>


// Qt includes
#include <QtNetwork/QNetworkReply>


namespace imtcom
{


// public methods

CInternetConnectionCheckerComp::CInternetConnectionCheckerComp()
	:m_status(ICS_UNKNOWN),
	m_managerPtr(nullptr),
	m_timeout(1000),
	m_delay(1000)
{
}

// reimplemented (IInternetConnectionStatusProvider)

imtcom::IInternetConnectionStatusProvider::InternetConnectionStatus CInternetConnectionCheckerComp::GetIntenetConnectionStatus() const
{
	return m_status;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CInternetConnectionCheckerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (*m_timeoutAttrPtr > 0){
		m_timeout = *m_timeoutAttrPtr;
	}

	if (*m_delayAttrPtr > 0){
		m_delay = *m_delayAttrPtr;
	}

	m_managerPtr = new QNetworkAccessManager();

	connect(&m_timer, &QTimer::timeout, this, &CInternetConnectionCheckerComp::OnTimer);
	m_timer.setSingleShot(true);
	m_timer.setInterval(m_delay);

	SendRequest();
}


void CInternetConnectionCheckerComp::OnComponentDestroyed()
{
	m_timer.stop();

	if (m_managerPtr != nullptr){
		m_managerPtr->deleteLater();
		m_managerPtr = nullptr;
	}

	BaseClass::OnComponentDestroyed();
}


// private Q_SLOTS

void CInternetConnectionCheckerComp::OnTimer()
{
	SendRequest();
}


void CInternetConnectionCheckerComp::OnRequestFinished()
{
	QNetworkReply* replyPtr = dynamic_cast<QNetworkReply*>(sender());
	if (replyPtr != nullptr){
		bool isOnline = replyPtr->error() == QNetworkReply::NoError;
		if (isOnline && m_status != ICS_ONLINE){
			istd::CChangeNotifier notifier(this);

			m_status = ICS_ONLINE;
		}
		else if (!isOnline && m_status != ICS_OFFLINE){
			istd::CChangeNotifier notifier(this);

			m_status = ICS_OFFLINE;
		}

		replyPtr->deleteLater();

		m_timer.start();
	}
}


// private methods

void CInternetConnectionCheckerComp::SendRequest()
{
	if (m_managerPtr != nullptr){
		QNetworkRequest request;
		request.setUrl(*m_urlAttrPtr);

#if QT_VERSION > QT_VERSION_CHECK(5, 14, 0)
		request.setTransferTimeout(m_timeout);
#endif
		QNetworkReply* replyPtr = m_managerPtr->get(request);
		if (replyPtr != nullptr){
			connect(replyPtr, &QNetworkReply::finished, this, &CInternetConnectionCheckerComp::OnRequestFinished);
		}
		else{
			m_timer.start();
		}
	}
}


} // namespace imtcom


