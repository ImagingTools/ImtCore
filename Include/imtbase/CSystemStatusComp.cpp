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
	m_textParamObserver(*this),
	m_blockAutoStart(false)
{
}


// reimplemented (ISystemStatus)

ISystemStatus::SystemStatus CSystemStatusComp::GetSystemStatus(QString& errorMessage) const
{
	errorMessage = m_statusMessage;

	return m_status;
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
		m_textParamObserver.RegisterObject(m_urlParamCompPtr.GetPtr(), &CSystemStatusComp::OnUrlParamChanged);
	}
	else{
		m_timer.start();
	}
}


void CSystemStatusComp::OnComponentDestroyed()
{
	m_checkStatusFutureWatcher.waitForFinished();
	m_textParamObserver.UnregisterAllObjects();

	BaseClass::OnComponentDestroyed();
}


void CSystemStatusComp::OnUrlParamChanged(
			const istd::IChangeable::ChangeSet& /*changeSet*/,
			const iprm::ITextParam* textParamPtr){
	Q_ASSERT(textParamPtr != nullptr);
	if (textParamPtr != nullptr){
		QString textParam = textParamPtr->GetText();

		m_workingUrl = textParam.toUtf8();

		m_statusMessage = QObject::tr("Try connection to the address %1 ...").arg(qPrintable(m_workingUrl));

		SetStatus(ISystemStatus::SS_TRY_CONNECTING_SERVER);

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

	if (*m_autoCheckStatusAttrPtr){
		int interval = m_checkIntervalAttrPtr.IsValid() ? *m_checkIntervalAttrPtr * 1000 : 60000;
		m_timer.start(interval);
	}
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
			m_statusMessage = QObject::tr("The %1 server cannot be connected at %2.").arg(qPrintable(*m_serverNameAttrPtr)).arg(qPrintable(m_workingUrl));
			m_futureResultStatus = ISystemStatus::SS_SERVER_CONNECTION_ERROR;

			return;
		}

		if (m_dbServerConnectionCheckerCompPtr.IsValid()){
			QString error;
			bool isConnected = m_dbServerConnectionCheckerCompPtr->IsDatabaseServerConnected(error);
			if (!isConnected){
				error = QString("%1 %2").arg(qPrintable(*m_serverNameAttrPtr)).arg(error);
				m_statusMessage = error;
				m_futureResultStatus = ISystemStatus::SS_DATABASE_CONNECTION_ERROR;

				return;
			}
		}

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


