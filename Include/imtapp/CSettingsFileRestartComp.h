// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QFileSystemWatcher>
#include <QtCore/QTimer>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iser/ISerializable.h>
#include <ifile/IFilePersistence.h>
#include <ifile/IFileNameParam.h>


namespace imtapp
{


/**
	Restarts the application when its settings file was changed by another process (e.g. a configurator).
	The new settings are loaded into the settings object before the restart, so storing on shutdown keeps them.
	Writes of the application itself leave the settings object unchanged and do not cause a restart.
*/
class CSettingsFileRestartComp: public QObject, public ilog::CLoggerComponentBase
{
	Q_OBJECT
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSettingsFileRestartComp);
		I_ASSIGN(m_settingsCompPtr, "Settings", "Settings object persisted in the observed file", true, "Settings");
		I_ASSIGN(m_fileLoaderCompPtr, "FileLoader", "File persistence used to store the settings", true, "FileLoader");
		I_ASSIGN(m_filePathCompPtr, "FilePath", "Path of the settings file", true, "FilePath");
		I_ASSIGN(m_checkDelayAttrPtr, "CheckDelay", "Delay in milliseconds after the last file change before the file is checked", true, 1000);
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private Q_SLOTS:
	void OnFileSystemChanged();
	void OnCheckSettingsFile();

private:
	void WatchSettingsFile();
	QByteArray ReadSettingsFile() const;
	QByteArray GetSettingsState() const;

private:
	I_REF(iser::ISerializable, m_settingsCompPtr);
	I_REF(ifile::IFilePersistence, m_fileLoaderCompPtr);
	I_REF(ifile::IFileNameParam, m_filePathCompPtr);
	I_ATTR(int, m_checkDelayAttrPtr);

	QFileSystemWatcher m_fileWatcher;
	QTimer m_checkTimer;
	QString m_filePath;
	QByteArray m_lastFileContents;
};


} // namespace imtapp


