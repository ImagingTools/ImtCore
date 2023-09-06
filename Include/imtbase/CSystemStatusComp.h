#pragma once


// Qt includes
#include <QtCore/QThread>
#include <QtCore/QTimer>
#include <QtCore/QFutureWatcher>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/ISystemStatus.h>
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
		I_ASSIGN(m_connectionStatusProviderCompPtr, "ConnectionStatusProvider", "Server connection status provider", true, "ConnectionStatusProvider");
		I_ASSIGN(m_dbServerConnectionCheckerCompPtr, "DatabaseServerConnectionChecker", "Database server connection status provider", true, "DatabaseServerConnectionChecker");
		I_ASSIGN(m_slaveSystemStatusCompPtr, "SlaveSystemStatus", "Slave system status", false, "SlaveSystemStatus");
		I_ASSIGN(m_checkIntervalAttrPtr, "CheckInterval", "Interval for backup timer (in sec)", false, 60);
		I_ASSIGN(m_autoCheckStatusAttrPtr, "AutoCheckStatus", "Auto-check system status", false, false);
		I_ASSIGN(m_serverNameAttrPtr, "ServerName", "The name of the server to which the connection is being checked", false, "");
	I_END_COMPONENT;

	CSystemStatusComp();

	// reimplemented (ISystemStatus)
	virtual SystemStatus GetSystemStatus(QString& errorMessage) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	void SetStatus(SystemStatus status);
	void OnCheckStatusFinished();
	void OnTimeout();
	void CheckStatus();

private:
	SystemStatus m_status;
	QString m_errorMessage;
	QTimer m_timer;
	QFutureWatcher<void> m_checkStatusFutureWatcher;
	SystemStatus m_futureResultStatus;

private:
	I_REF(imtcom::IConnectionStatusProvider, m_connectionStatusProviderCompPtr);
	I_REF(imtdb::IDatabaseServerConnectionChecker, m_dbServerConnectionCheckerCompPtr);
	I_REF(imtbase::ISystemStatus, m_slaveSystemStatusCompPtr);
	I_ATTR(int, m_checkIntervalAttrPtr);
	I_ATTR(bool, m_autoCheckStatusAttrPtr);
	I_ATTR(QByteArray, m_serverNameAttrPtr);
};


} // namespace imtbase


