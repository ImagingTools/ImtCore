#pragma once


// Qt includes
#include <QtCore/QTimer>
#include <QtNetwork/QNetworkAccessManager>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtcom/IInternetConnectionStatusProvider.h>


namespace imtcom
{


class CInternetConnectionCheckerComp:
			public QObject,
			public ilog::CLoggerComponentBase,
			virtual public IInternetConnectionStatusProvider
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CInternetConnectionCheckerComp);
		I_REGISTER_INTERFACE(IInternetConnectionStatusProvider);
		I_ASSIGN(m_urlAttrPtr, "Url", "URL to check availability", true, "https://google.com");
		I_ASSIGN(m_requestTimeoutAttrPtr, "RequestTimeout", "Request timeout", true, 1000)
		I_ASSIGN(m_requestDelayAttrPtr, "RequestDelay", "Delay between two consecutive requests", true, 1000);
		I_ASSIGN(m_retryCountAttrPtr, "RetryCount", "Number of retries after failure before going offline", true, 10);
	I_END_COMPONENT;

	CInternetConnectionCheckerComp();

	// reimplemented (IInternetConnectionStatusProvider)
	virtual InternetConnectionStatus GetIntenetConnectionStatus() const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private Q_SLOTS:
	void OnTimer();
	void OnRequestFinished();

private:
	void SendRequest();

private:
	I_ATTR(QString, m_urlAttrPtr);
	I_ATTR(int, m_requestTimeoutAttrPtr);
	I_ATTR(int, m_requestDelayAttrPtr);
	I_ATTR(int, m_retryCountAttrPtr);

	InternetConnectionStatus m_status;
	QTimer m_timer;
	QNetworkAccessManager* m_managerPtr;

	int m_requestTimeout;
	int m_requestDelay;
	int m_retryCount;
	int m_retryCounter;
};


} // namespace imtcom


