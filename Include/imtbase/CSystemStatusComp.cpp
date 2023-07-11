#include <imtbase/CSystemStatusComp.h>


// Qt includes
#if QT_VERSION >= 0x050000
#include <QtConcurrent/QtConcurrent>
#else
#include <QtCore>
#endif


namespace imtbase
{


// public methods

CSystemStatusComp::CSystemStatusComp()
	:m_status(SystemStatus::SS_UNKNOWN_ERROR),
	m_futureResultStatus(SystemStatus::SS_UNKNOWN_ERROR)
{
}


// reimplemented (ISystemStatus)

ISystemStatus::SystemStatus CSystemStatusComp::GetSystemStatus(QString& errorMessage) const
{
	if (m_status == ISystemStatus::SystemStatus::SS_SERVER_CONNECTION_ERROR){
		errorMessage = tr("Server connection error");
	}
	else if (m_status == ISystemStatus::SystemStatus::SS_DATABASE_CONNECTION_ERROR){
		errorMessage = m_errorMessage;
	}
	else if (m_status == ISystemStatus::SystemStatus::SS_UNKNOWN_ERROR){
		errorMessage = tr("Unknown error");
	}

	return m_status;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CSystemStatusComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	connect(&m_timer, &QTimer::timeout, this, &CSystemStatusComp::OnTimeout);
	connect(&m_checkStatusFutureWatcher, &QFutureWatcher<void>::finished, this, &CSystemStatusComp::OnCheckStatusFinished);

	m_timer.setSingleShot(true);
	m_timer.start(0);
}


void CSystemStatusComp::OnComponentDestroyed()
{
	m_checkStatusFutureWatcher.waitForFinished();

	BaseClass::OnComponentDestroyed();
}


// private methods

void CSystemStatusComp::SetStatus(ISystemStatus::SystemStatus status)
{
	if (m_status != status){
		istd::CChangeNotifier notifier(this);
		Q_UNUSED(notifier);

		m_status = status;
	}
}


// private slots

void CSystemStatusComp::OnCheckStatusFinished()
{
	SetStatus(m_futureResultStatus);

	int interval = m_checkIntervalAttrPtr.IsValid() ? *m_checkIntervalAttrPtr * 1000 : 60000;
	m_timer.start(interval);
}


void CSystemStatusComp::OnTimeout()
{
	Q_ASSERT(!m_checkStatusFutureWatcher.isRunning());

#if QT_VERSION >= 0x060000
	m_checkStatusFutureWatcher.setFuture(QtConcurrent::run(&CSystemStatusComp::CheckStatus, this));
#else
	m_checkStatusFutureWatcher.setFuture(QtConcurrent::run(this, &CSystemStatusComp::CheckStatus));
#endif
}


void CSystemStatusComp::CheckStatus()
{
	if (m_connectionStatusProviderCompPtr.IsValid()){
		imtcom::IConnectionStatusProvider::ConnectionStatus serverConnectionStatus = m_connectionStatusProviderCompPtr->GetConnectionStatus();
		if (serverConnectionStatus != imtcom::IConnectionStatusProvider::ConnectionStatus::CS_CONNECTED){
			m_futureResultStatus = ISystemStatus::SystemStatus::SS_SERVER_CONNECTION_ERROR;

			return;
		}

		if (m_dbServerConnectionCheckerCompPtr.IsValid()){
			QString error;
			bool isConnected = m_dbServerConnectionCheckerCompPtr->IsDatabaseServerConnected(error);
			if (!isConnected){
				m_errorMessage = error;

				m_futureResultStatus = ISystemStatus::SystemStatus::SS_DATABASE_CONNECTION_ERROR;

				return;
			}
		}

		m_futureResultStatus = ISystemStatus::SystemStatus::SS_NO_ERROR;
	}
}


} // namespace imtbase


