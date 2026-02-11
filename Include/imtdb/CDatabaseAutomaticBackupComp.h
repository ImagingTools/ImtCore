// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QTimer>
#include <QtCore/QFutureWatcher>

// ACF includes
#include <icomp/CComponentBase.h>
#include <ifile/IFileNameParam.h>
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFilePersistence.h>
#include <ifile/IRelativeFilePath.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtdb/IDatabaseLoginSettings.h>
#include <imtapp/IBackupSettings.h>


namespace imtdb
{


class CDatabaseAutomaticBackupComp:
			public QObject,
			public ilog::CLoggerComponentBase
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDatabaseAutomaticBackupComp);
		I_ASSIGN(m_databaseLoginSettingsCompPtr, "DatabaseLoginSettings", "Database login settings", false, "DatabaseLoginSettings");
		I_ASSIGN(m_backupSettingsCompPtr, "BackupSettings", "Backup settings", false, "BackupSettings");
		I_ASSIGN(m_programAttrPtr, "Program", "Program name", true, "Program");
		I_ASSIGN(m_checkIntervalAttrPtr, "CheckInterval", "Interval for backup timer (in msec)", true, 60000);
		I_ASSIGN(m_maxBackupCountAttrPtr, "MaxBackupCount", "Maximum number of backups, if -1 - unlimited", true, 20);
		I_ASSIGN(m_backupOnStartAttrPtr, "BackupOnStart", "Backup on start", true, true);
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	bool Backup();
	void CleanupOldBackups(const QString& backupFolderPath);
	QStringList GetPrevBackupsList(const QString& backupFolderPath, const QDir::SortFlags& sortFlags = QDir::Time | QDir::Reversed);

private Q_SLOTS:
	void OnTimeout();
	void OnBackupFinished();

protected:
	QTimer m_timer;

protected:
	I_REF(imtdb::IDatabaseLoginSettings, m_databaseLoginSettingsCompPtr);
	I_REF(iprm::IParamsSet, m_backupSettingsCompPtr);
	I_ATTR(QString, m_programAttrPtr);
	I_ATTR(int, m_checkIntervalAttrPtr);
	I_ATTR(int, m_maxBackupCountAttrPtr);
	I_ATTR(bool, m_backupOnStartAttrPtr);

private:
	QDateTime m_lastBackupDateTime;

	QFutureWatcher<bool> m_backupWatcher;
};


} // namespace imtdb


