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
	m_futureResultStatus(SystemStatus::SS_UNKNOWN_ERROR),
	m_urlParamObserver(*this),
	m_singleCheck(false),
	m_autoCheck(true)
{
}


// reimplemented (ISystemStatus)

ISystemStatus::SystemStatus CSystemStatusComp::GetSystemStatus(QString& errorMessage) const
{
	errorMessage = m_statusMessage;

	return m_status;
}


void CSystemStatusComp::UpdateSystemStatus()
{
	if (m_slaveSystemStatusCompPtr.IsValid()){
		m_slaveSystemStatusCompPtr->UpdateSystemStatus();
	}

	QString urlString = m_workingUrl.toString();

	m_statusMessage = tr("Try connect to: '%1' ...").arg(urlString);

	SetStatus(ISystemStatus::SS_TRY_CONNECTING_SERVER);

	StartCheckSystemStatus();
}


imtcom::IConnectionStatusProvider* CSystemStatusComp::GetConnectionStatusProvider() const
{
	return m_connectionStatusProviderCompPtr.GetPtr();
}


imtdb::IDatabaseServerConnectionChecker* CSystemStatusComp::GetDatabaseServerConnectionStatusProvider() const
{
	return m_dbServerConnectionCheckerCompPtr.GetPtr();
}


bool CSystemStatusComp::StartCheckSystemStatus()
{
	m_autoCheck = true;

	if (!m_timer.isActive()){
		m_timer.start();
	}

	return true;
}


bool CSystemStatusComp::StopCheckSystemStatus()
{
	m_timer.stop();

	m_autoCheck = false;

	return true;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CSystemStatusComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_slaveSystemStatusCompPtr.EnsureInitialized();

	m_timer.setSingleShot(true);

	connect(&m_timer, &QTimer::timeout, this, &CSystemStatusComp::OnTimeout);
	connect(&m_checkStatusFutureWatcher, &QFutureWatcher<void>::finished, this, &CSystemStatusComp::OnCheckStatusFinished);

	if (m_urlParamCompPtr.IsValid()){
		m_urlParamObserver.RegisterObject(m_urlParamCompPtr.GetPtr(), &CSystemStatusComp::OnUrlParamChanged);
	}
	else{
		m_timer.start();
	}
}


void CSystemStatusComp::OnComponentDestroyed()
{
	m_checkStatusFutureWatcher.waitForFinished();

	m_urlParamObserver.UnregisterAllObjects();

	BaseClass::OnComponentDestroyed();
}


void CSystemStatusComp::OnUrlParamChanged(
			const istd::IChangeable::ChangeSet& /*changeSet*/,
			const imtbase::IUrlParam* urlParamPtr)
{
	Q_ASSERT(urlParamPtr != nullptr);
	if (urlParamPtr != nullptr){
		m_workingUrl = urlParamPtr->GetUrl();

		QString urlString = m_workingUrl.toString();

		m_statusMessage = tr("Try connect to: '%1' ...").arg(urlString);

//		SetStatus(ISystemStatus::SS_TRY_CONNECTING_SERVER);

		m_timer.start();
	}
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

	if (m_status == ISystemStatus::SS_NO_ERROR){
		StopCheckSystemStatus();

		return;
	}

	if (m_autoCheck && !m_singleCheck){
		int interval = m_checkIntervalAttrPtr.IsValid() ? *m_checkIntervalAttrPtr * 1000 : 60000;
		m_timer.start(interval);
	}

	if (m_singleCheck){
		m_singleCheck = false;
	}
}


void CSystemStatusComp::OnTimeout()
{
	if (m_checkStatusFutureWatcher.isRunning()){
		return;
	}

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
			m_statusMessage = QObject::tr("The %1 server cannot be connected at %2.").arg(qPrintable(*m_serverNameAttrPtr)).arg(m_workingUrl.toString());
			m_futureResultStatus = ISystemStatus::SS_SERVER_CONNECTION_ERROR;

			return;
		}

		if (m_dbServerConnectionCheckerCompPtr.IsValid()){
			QString error;
			bool isConnected = m_dbServerConnectionCheckerCompPtr->CheckDatabaseConnection(error);
			if (!isConnected){
				error = QString("%1 %2").arg(qPrintable(*m_serverNameAttrPtr)).arg(error);
				m_statusMessage = error;
				m_futureResultStatus = ISystemStatus::SS_DATABASE_CONNECTION_ERROR;

				return;
			}
		}

		m_statusMessage = "";
		m_futureResultStatus = ISystemStatus::SS_NO_ERROR;
	}

	if (m_slaveSystemStatusCompPtr.IsValid()){
		QString error;
		ISystemStatus::SystemStatus slaveStatus = m_slaveSystemStatusCompPtr->GetSystemStatus(error);
		if (slaveStatus != SS_NO_ERROR){
			m_statusMessage = error;

			m_futureResultStatus = slaveStatus;
		}
	}
}


} // namespace imtbase


