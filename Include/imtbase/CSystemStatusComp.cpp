#include <imtbase/CSystemStatusComp.h>


namespace imtbase
{


// public methods

CSystemStatusComp::CSystemStatusComp()
	:m_status(SystemStatus::SS_UNKNOWN_ERROR),
	m_checkStatusThread(*this)
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

	connect(&m_checkStatusThread, &QThread::finished, this, &CSystemStatusComp::OnCheckStatusFinished, Qt::QueuedConnection);

	if (m_connectionStatusProviderCompPtr.IsValid() && m_dbServerConnectionCheckerCompPtr.IsValid()){
		m_checkStatusThread.Start();
	}
}


void CSystemStatusComp::OnComponentDestroyed()
{
	if (m_checkStatusThread.isRunning()){
		m_checkStatusThread.requestInterruption();
		m_checkStatusThread.wait();
	}

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


void CSystemStatusComp::CheckStatus()
{
	if (m_connectionStatusProviderCompPtr.IsValid()){
		imtcom::IConnectionStatusProvider::ConnectionStatus serverConnectionStatus = m_connectionStatusProviderCompPtr->GetConnectionStatus();
		if (serverConnectionStatus != imtcom::IConnectionStatusProvider::ConnectionStatus::CS_CONNECTED){
			SetStatus(ISystemStatus::SystemStatus::SS_SERVER_CONNECTION_ERROR);

			return;
		}

		if (m_dbServerConnectionCheckerCompPtr.IsValid()){
			bool isConnected = m_dbServerConnectionCheckerCompPtr->IsDatabaseServerConnected(m_errorMessage);
			if (!isConnected){
				SetStatus(ISystemStatus::SystemStatus::SS_DATABASE_CONNECTION_ERROR);

				return;
			}
		}

		SetStatus(ISystemStatus::SystemStatus::SS_NO_ERROR);
	}
}


// private slots

void CSystemStatusComp::OnCheckStatusFinished()
{
	ISystemStatus::SystemStatus systemStatus = m_checkStatusThread.GetStatus();

	m_errorMessage = m_checkStatusThread.GetErrorMessage();

	SetStatus(systemStatus);

	m_checkStatusThread.Start();
}


// public methods of the embedded class CheckStatusThread

CSystemStatusComp::CheckStatusThread::CheckStatusThread(CSystemStatusComp& parent)
	:m_parent(parent)
{

}


void CSystemStatusComp::CheckStatusThread::Start()
{
	QThread::start();
}


ISystemStatus::SystemStatus CSystemStatusComp::CheckStatusThread::GetStatus()
{
	return m_status;
}


QString CSystemStatusComp::CheckStatusThread::GetErrorMessage()
{
	return m_errorMessage;
}


// protected methods of the embedded class CheckStatusThread

// reimplemented (QThread)

void CSystemStatusComp::CheckStatusThread::run()
{
	if (m_parent.m_connectionStatusProviderCompPtr.IsValid()){
		imtcom::IConnectionStatusProvider::ConnectionStatus serverConnectionStatus = m_parent.m_connectionStatusProviderCompPtr->GetConnectionStatus();
		if (serverConnectionStatus != imtcom::IConnectionStatusProvider::ConnectionStatus::CS_CONNECTED){
			m_status = ISystemStatus::SystemStatus::SS_SERVER_CONNECTION_ERROR;

			return;
		}

		if (m_parent.m_dbServerConnectionCheckerCompPtr.IsValid()){
			QString error;
			bool isConnected = m_parent.m_dbServerConnectionCheckerCompPtr->IsDatabaseServerConnected(error);
			if (!isConnected){
				m_errorMessage = error;
				m_status = ISystemStatus::SystemStatus::SS_DATABASE_CONNECTION_ERROR;

				return;
			}
		}

		m_status = ISystemStatus::SystemStatus::SS_NO_ERROR;
	}
}


} // namespace imtbase


