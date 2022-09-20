#include <imtcom/CInternetConnectionCheckerComp.h>


// Qt includes
#include <QtNetwork/QNetworkReply>


namespace imtcom
{


// public methods

CInternetConnectionCheckerComp::CInternetConnectionCheckerComp()
	:m_status(ICS_UNKNOWN),
	m_managerPtr(nullptr),
	m_requestTimeout(1000),
	m_requestDelay(1000),
	m_retryCount(0),
	m_retryCounter(0)
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

	if (*m_requestTimeoutAttrPtr> 0){
		m_requestTimeout= *m_requestTimeoutAttrPtr;
	}

	if (*m_requestDelayAttrPtr > 0){
		m_requestDelay= *m_requestDelayAttrPtr;
	}

	if (*m_retryCountAttrPtr> 0){
		m_retryCount = *m_retryCountAttrPtr;
	}

	m_managerPtr = new QNetworkAccessManager();

#if QT_VERSION > QT_VERSION_CHECK(5, 15, 0)
	m_managerPtr->setTransferTimeout(m_requestTimeout);
#endif 
	connect(&m_timer, &QTimer::timeout, this, &CInternetConnectionCheckerComp::OnTimer);
	m_timer.setSingleShot(true);
	m_timer.setInterval(m_requestDelay);

	SendRequest();
}


void CInternetConnectionCheckerComp::OnComponentDestroyed()
{
	m_timer.stop();

	if (m_managerPtr != nullptr){
		m_managerPtr->deleteLater();
		m_managerPtr = nullptr;
	}

	disconnect();

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
		bool result = replyPtr->error() == QNetworkReply::NoError;
		replyPtr->deleteLater();

		if (result){
			if (m_status != ICS_ONLINE){
				istd::CChangeNotifier notifier(this);

				m_status = ICS_ONLINE;
				SendVerboseMessage("Internet connection available");
			}
		}
		else{
			if (m_retryCounter < m_retryCount){
				m_retryCounter++;
				SendRequest();

				return;
			}
			else{
				if (m_status != ICS_OFFLINE){
					istd::CChangeNotifier notifier(this);

					m_status = ICS_OFFLINE;
					SendVerboseMessage("No internet connection");
				}
			}
		}
	}

	m_retryCounter = 0;
	m_timer.start();
}


// private methods

void CInternetConnectionCheckerComp::SendRequest()
{
	if (m_managerPtr != nullptr){
		QNetworkRequest request;
		request.setUrl(*m_urlAttrPtr);

		if (m_urlParamCompPtr.IsValid()){
			QUrl url = m_urlParamCompPtr->GetUrl();
			if (url.isValid() && (url.scheme() == "http" || url.scheme() == "https")){
				request.setUrl(url);
			}
		}

#if QT_VERSION > QT_VERSION_CHECK(5, 14, 0)
		request.setTransferTimeout(m_requestTimeout);
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


