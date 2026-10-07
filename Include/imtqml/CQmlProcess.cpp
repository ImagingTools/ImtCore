// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CQmlProcess.h>


namespace imtqml
{


// public methods

CQmlProcess::CQmlProcess()
	:m_state(QProcess::ProcessState::NotRunning),
	m_exitStatus(QProcess::NormalExit),
	m_exitCode(0)
{
	connect(&m_process, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(onStateChanged(QProcess::ProcessState)));
	connect(&m_process, SIGNAL(readyReadStandardError()), this, SLOT(onReadyReadStandardError()));
	connect(&m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(onReadyReadStandardOutput()));
	connect(&m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(onFinished(int, QProcess::ExitStatus)));
	connect(&m_process, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(onErrorOccurred(QProcess::ProcessError)));
}


CQmlProcess::~CQmlProcess()
{
	m_process.waitForFinished();
}


void CQmlProcess::start(QString command)
{
	m_process.start(command, m_arguments);

	emit started();
}


void CQmlProcess::start(QString command, QStringList arguments)
{
	m_process.start(command, arguments);
	emit started();
}


void CQmlProcess::addArgument(QString argument)
{
	m_arguments << argument;
}


void CQmlProcess::kill()
{
	m_process.kill();
}


void CQmlProcess::terminate()
{
	m_process.terminate();
}


void CQmlProcess::setEnviroment(QStringList enviroments)
{
	// Variables are added to the system environment: without SystemRoot child processes cannot even resolve host names on Windows.
	QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
	for (const QString& variable : std::as_const(enviroments)){
		const qsizetype separatorIndex = variable.indexOf(QLatin1Char('='));
		if (separatorIndex > 0){
			environment.insert(variable.left(separatorIndex), variable.mid(separatorIndex + 1));
		}
	}

	m_process.setProcessEnvironment(environment);
}


int CQmlProcess::getExitCode() const
{
	return m_exitCode;
}


void CQmlProcess::setExitCode(int exitCode)
{
	if (m_exitCode != exitCode){
		m_exitCode = exitCode;

		emit exitCodeChanged();
	}
}


QProcess::ProcessState CQmlProcess::getState() const
{
	return m_state;
}


void CQmlProcess::setState(QProcess::ProcessState state)
{
	if (m_state != state){
		m_state = state;

		emit stateChanged();
	}
}


QProcess::ExitStatus CQmlProcess::getExitStatus() const
{
	return m_exitStatus;
}


void CQmlProcess::setExitStatus(QProcess::ExitStatus status)
{
	if (m_exitStatus != status){
		m_exitStatus = status;

		emit exitStatusChanged();
	}
}


// public Q_SLOTS

void CQmlProcess::onStateChanged(QProcess::ProcessState newState)
{
	setState(newState);
}


void CQmlProcess::onReadyReadStandardError()
{
	QProcess* processPtr = dynamic_cast<QProcess*>(sender());
	if (processPtr != nullptr){
		emit standardError(processPtr->readAllStandardError());
	}
}


void CQmlProcess::onReadyReadStandardOutput()
{
	QProcess* processPtr = dynamic_cast<QProcess*>(sender());
	if (processPtr != nullptr){
		emit standardOutput(processPtr->readAllStandardOutput());
	}
}


void CQmlProcess::onFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	setExitCode(exitCode);
	setExitStatus(exitStatus);

	emit finished();
}


void CQmlProcess::onErrorOccurred(QProcess::ProcessError processError)
{
	// A process that never started does not emit finished(); report it as a failed run.
	if (processError == QProcess::FailedToStart){
		setExitCode(-1);
		setExitStatus(QProcess::CrashExit);

		emit standardError(m_process.errorString());
		emit error();
		emit finished();
	}
}


} // namespace imtqml


