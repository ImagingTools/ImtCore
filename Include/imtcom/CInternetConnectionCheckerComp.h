#pragma once


// Qt includes
#include <QtCore/QTimer>
#include <QtNetwork/QNetworkAccessManager>

// ACF includes
#include <iprm/IEnableableParam.h>
#include <icomp/CComponentBase.h>


namespace imtcom
{


class CInternetConnectionCheckerComp:
			public QObject,
			public icomp::CComponentBase,
			virtual public iprm::IEnableableParam
{
	Q_OBJECT
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CInternetConnectionCheckerComp);
		I_REGISTER_INTERFACE(iprm::IEnableableParam);
		I_ASSIGN(m_urlAttrPtr, "Url", "URL to check availability", true, "https://google.com");
		I_ASSIGN(m_timeoutAttrPtr, "Timeout", "Request timeout", true, 1000)
		I_ASSIGN(m_delayAttrPtr, "Delay", "Delay between two consecutive requests", true, 1000);
	I_END_COMPONENT;

	CInternetConnectionCheckerComp();

	// reimplemented (iprm::IEnableableParam)
	virtual bool IsEnabled() const override;
	virtual bool IsEnablingAllowed() const override;
	virtual bool SetEnabled(bool isEnabled = true) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

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

	bool m_isOnline;
	QTimer m_timer;
	QNetworkAccessManager* m_managerPtr;

	int m_timeout;
	int m_delay;
};


} // namespace imtcom


