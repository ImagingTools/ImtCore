#pragma once


// Qt includes
#include <QtCore/QThread>
#include <QtCore/QTimer>
#include <QtCore/QFutureWatcher>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/ISystemStatus.h>
#include <imtbase/TModelUpdateBinder.h>
#include <imtbase/IUrlParam.h>
#include <imtcom/IConnectionStatusProvider.h>
#include <imtdb/IDatabaseServerConnectionChecker.h>


namespace imtbase
{


class CSystemStatusComp:
			public QObject,
			public ilog::CLoggerComponentBase,
			virtual public imtbase::ISystemStatus
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSystemStatusComp);
		I_REGISTER_INTERFACE(imtbase::ISystemStatus);
		I_ASSIGN(m_connectionStatusProviderCompPtr, "ConnectionStatusProvider", "Server connection status provider", false, "ConnectionStatusProvider");
		I_ASSIGN(m_dbServerConnectionCheckerCompPtr, "DatabaseServerConnectionChecker", "Database server connection status provider", false, "DatabaseServerConnectionChecker");
		I_ASSIGN(m_slaveSystemStatusCompPtr, "SlaveSystemStatus", "Slave system status", false, "SlaveSystemStatus");
		I_ASSIGN(m_checkIntervalAttrPtr, "CheckInterval", "Interval for backup timer (in sec)", false, 60);
		I_ASSIGN(m_checkDbStatusAttrPtr, "CheckDatabaseStatus", "Check database system status", false, true);
		I_ASSIGN(m_serverNameAttrPtr, "ServerName", "The name of the server to which the connection is being checked", false, "");
		I_ASSIGN(m_urlParamCompPtr, "UrlParam", "The object holds connection's url.", false, "UrlParam");
	I_END_COMPONENT;

	CSystemStatusComp();

	// reimplemented (ISystemStatus)
	virtual SystemStatus GetSystemStatus(QString& errorMessage) const override;
	virtual void UpdateSystemStatus() override;
	virtual imtcom::IConnectionStatusProvider* GetConnectionStatusProvider() const override;
	virtual imtdb::IDatabaseServerConnectionChecker* GetDatabaseServerConnectionStatusProvider() const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	void OnUrlParamChanged(const istd::IChangeable::ChangeSet& changeSet, const imtbase::IUrlParam* urlParamPtr);

private:
	virtual bool StartCheckSystemStatus();
	virtual bool StopCheckSystemStatus();
	void SetStatus(SystemStatus status);
	void OnCheckStatusFinished();
	void OnTimeout();
	void CheckStatus();

private:
	SystemStatus m_status;
	QString m_statusMessage;
	QTimer m_timer;
	QFutureWatcher<void> m_checkStatusFutureWatcher;
	SystemStatus m_futureResultStatus;
	imtbase::TModelUpdateBinder<imtbase::IUrlParam, CSystemStatusComp> m_urlParamObserver;
	QUrl m_workingUrl;

private:
	I_REF(imtcom::IConnectionStatusProvider, m_connectionStatusProviderCompPtr);
	I_REF(imtdb::IDatabaseServerConnectionChecker, m_dbServerConnectionCheckerCompPtr);
	I_REF(imtbase::ISystemStatus, m_slaveSystemStatusCompPtr);
	I_ATTR(int, m_checkIntervalAttrPtr);
	I_ATTR(bool, m_checkDbStatusAttrPtr);
	I_ATTR(QByteArray, m_serverNameAttrPtr);
	I_REF(imtbase::IUrlParam, m_urlParamCompPtr);
};


} // namespace imtbase


