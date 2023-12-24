#pragma once


// Qt includes
#include <QtCore/QTimer>
#include <QtNetwork/QNetworkAccessManager>

// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IUrlParam.h>
#include <imtcom/IConnectionStatusProvider.h>


namespace imtcom
{


class CAsyncConnectionCheckerComp:
			public QObject,
			public icomp::CComponentBase,
			virtual public IConnectionStatusProvider
{
	Q_OBJECT
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CAsyncConnectionCheckerComp);
		I_REGISTER_INTERFACE(IConnectionStatusProvider);
		I_ASSIGN(m_prefixServer, "ServerPrefix", "Server prefix", true, "ServerPrefix");
		I_ASSIGN(m_urlParamCompPtr, "UrlParam", "The object holds connection's url.", false, "UrlParam");
		I_ASSIGN(m_checkIntervalAttrPtr, "CheckInterval", "Interval for backup timer (in sec)", false, 60);
	I_END_COMPONENT;

	CAsyncConnectionCheckerComp();

	void SendRequest();
	void SetStatus(ConnectionStatus status);

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (IConnectionStatusProvider)
	virtual ConnectionStatus GetConnectionStatus() const override;

public Q_SLOTS:
	void ReplyFinished();
	void OnTimeout();

private:
	ConnectionStatus m_status;
	QTimer m_timer;
	QNetworkAccessManager* m_managerPtr;

private:
	I_ATTR(QByteArray, m_urlAttrPtr);
	I_REF(imtbase::IUrlParam, m_urlParamCompPtr);
	I_ATTR(QByteArray, m_prefixServer);
	I_ATTR(int, m_checkIntervalAttrPtr);
};


} // namespace imtcom


