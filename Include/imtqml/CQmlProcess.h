// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QObject>
#include <QtCore/QProcess>


namespace imtqml
{


class CQmlProcess : public QObject
{
	Q_OBJECT

	Q_PROPERTY(int exitCode READ getExitCode WRITE setExitCode NOTIFY exitCodeChanged)
	Q_PROPERTY(QProcess::ProcessState state READ getState WRITE setState NOTIFY stateChanged)
	Q_PROPERTY(QProcess::ExitStatus exitStatus READ getExitStatus WRITE setExitStatus NOTIFY exitStatusChanged)

public:
	CQmlProcess();
	~CQmlProcess();

	Q_INVOKABLE void start(QString command);
	Q_INVOKABLE void start(QString command, QStringList arguments);
	Q_INVOKABLE void addArgument(QString argument);
	Q_INVOKABLE void kill();
	Q_INVOKABLE void terminate();
	Q_INVOKABLE void setEnviroment(QStringList enviroments);

	int getExitCode() const;
	void setExitCode(int exitCode);
	QProcess::ProcessState getState() const;
	void setState(QProcess::ProcessState state);
	QProcess::ExitStatus getExitStatus() const;
	void setExitStatus(QProcess::ExitStatus status);

Q_SIGNALS:
	void finished();
	void error();
	void started();
	void standardError(QString error);
	void standardOutput(QString error);
	void exitCodeChanged();
	void stateChanged();
	void exitStatusChanged();

public Q_SLOTS:
	void onStateChanged(QProcess::ProcessState newState);
	void onReadyReadStandardError();
	void onReadyReadStandardOutput();
	void onFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
	QStringList m_arguments;
	QProcess m_process;
	QProcess::ProcessState m_state;
	QProcess::ExitStatus m_exitStatus;
	int m_exitCode;
};


} // namespace imtqml


Q_DECLARE_METATYPE(imtqml::CQmlProcess*)


