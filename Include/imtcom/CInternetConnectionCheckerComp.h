#pragma once


// Qt includes
#include <QtCore/QTimer>
#include <QtNetwork/QNetworkAccessManager>

// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtcom/IInternetConnectionStatusProvider.h>


namespace imtcom
{


class CInternetConnectionCheckerComp:
			public QObject,
			public icomp::CComponentBase,
			virtual public IInternetConnectionStatusProvider
{
	Q_OBJECT
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CInternetConnectionCheckerComp);
		I_REGISTER_INTERFACE(IInternetConnectionStatusProvider);
		I_ASSIGN(m_urlAttrPtr, "Url", "URL to check availability", true, "https://google.com");
		I_ASSIGN(m_timeoutAttrPtr, "Timeout", "Request timeout", true, 1000)
		I_ASSIGN(m_delayAttrPtr, "Delay", "Delay between two consecutive requests", true, 1000);
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
	I_ATTR(int, m_timeoutAttrPtr);
	I_ATTR(int, m_delayAttrPtr);

	InternetConnectionStatus m_status;
	QTimer m_timer;
	QNetworkAccessManager* m_managerPtr;

	int m_timeout;
	int m_delay;
};


} // namespace imtcom


