// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtapp/CSettingsFileRestartComp.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

// ACF includes
#include <iser/CMemoryWriteArchive.h>

// ImtCore includes
#include <imtcore/CApplicationRunner.h>


namespace imtapp
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CSettingsFileRestartComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	const QString filePath = m_filePathCompPtr->GetPath();
	if (filePath.isEmpty()){
		return;
	}

	m_filePath = QFileInfo(filePath).absoluteFilePath();
	m_lastFileContents = ReadSettingsFile();

	m_checkTimer.setSingleShot(true);
	m_checkTimer.setInterval(*m_checkDelayAttrPtr);

	connect(&m_checkTimer, &QTimer::timeout, this, &CSettingsFileRestartComp::OnCheckSettingsFile);
	connect(&m_fileWatcher, &QFileSystemWatcher::fileChanged, this, &CSettingsFileRestartComp::OnFileSystemChanged);
	connect(&m_fileWatcher, &QFileSystemWatcher::directoryChanged, this, &CSettingsFileRestartComp::OnFileSystemChanged);

	WatchSettingsFile();
}


void CSettingsFileRestartComp::OnComponentDestroyed()
{
	m_checkTimer.stop();

	disconnect(&m_fileWatcher, nullptr, this, nullptr);
	disconnect(&m_checkTimer, nullptr, this, nullptr);

	BaseClass::OnComponentDestroyed();
}


// private slots

void CSettingsFileRestartComp::OnFileSystemChanged()
{
	m_checkTimer.start();
}


void CSettingsFileRestartComp::OnCheckSettingsFile()
{
	WatchSettingsFile();

	const QByteArray fileContents = ReadSettingsFile();
	if (fileContents.isEmpty() || fileContents == m_lastFileContents){
		return;
	}

	const QByteArray stateBefore = GetSettingsState();
	if (m_fileLoaderCompPtr->LoadFromFile(*m_settingsCompPtr, m_filePath) != ifile::IFilePersistence::OS_OK){
		SendWarningMessage(0, QStringLiteral("Unable to load changed settings file '%1'").arg(m_filePath));

		return;
	}

	m_lastFileContents = fileContents;

	// the application's own writes reproduce the state it already has
	if (GetSettingsState() == stateBefore){
		return;
	}

	SendInfoMessage(0, QStringLiteral("Settings file '%1' was changed, restarting the application").arg(m_filePath));

	if (!imtcore::CApplicationRunner::Restart()){
		SendErrorMessage(0, QStringLiteral("Unable to restart the application, new settings will be applied after the next start"));
	}
}


// private methods

void CSettingsFileRestartComp::WatchSettingsFile()
{
	// replacing the file by rename drops it from the watcher, the directory watch catches the replacement
	const QString directoryPath = QFileInfo(m_filePath).absolutePath();
	if (!m_fileWatcher.directories().contains(directoryPath) && QFileInfo::exists(directoryPath)){
		m_fileWatcher.addPath(directoryPath);
	}

	if (!m_fileWatcher.files().contains(m_filePath) && QFileInfo::exists(m_filePath)){
		m_fileWatcher.addPath(m_filePath);
	}
}


QByteArray CSettingsFileRestartComp::ReadSettingsFile() const
{
	QFile file(m_filePath);
	if (!file.open(QIODevice::ReadOnly)){
		return QByteArray();
	}

	return file.readAll();
}


QByteArray CSettingsFileRestartComp::GetSettingsState() const
{
	iser::CMemoryWriteArchive archive(nullptr, false);
	if (!m_settingsCompPtr->Serialize(archive)){
		return QByteArray();
	}

	return QByteArray(static_cast<const char*>(archive.GetBuffer()), archive.GetBufferSize());
}


} // namespace imtapp


