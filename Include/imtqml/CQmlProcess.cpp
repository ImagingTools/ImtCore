// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CQmlProcess.h>


namespace imtqml
{


// public methods

CQmlProcess::CQmlProcess()
	:m_state(QProcess::ProcessState::NotRunning)
{
	connect(&m_process, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(onStateChanged(QProcess::ProcessState)));
	connect(&m_process, SIGNAL(readyReadStandardError()), this, SLOT(onReadyReadStandardError()));
	connect(&m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(onReadyReadStandardOutput()));
	connect(&m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(onFinished(int, QProcess::ExitStatus)));
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
	m_process.setEnvironment(enviroments);
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


} // namespace imtqml


