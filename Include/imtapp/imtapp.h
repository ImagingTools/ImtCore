// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

/**
	\file imtapp.h
	\brief Main header file for the imtapp module - Desktop Application Framework
	
	This file provides namespace-level documentation for the imtapp module.
	Include this file to access the complete imtapp module documentation in Doxygen.
*/

/**
	\namespace imtapp
	\brief Desktop Application Framework Module
	
	The \b imtapp module provides high-level application framework components for
	desktop applications, including backup settings, scheduled tasks, and
	application configuration management.
	
	\section imtapp_overview Overview
	
	This module serves as a framework layer for building desktop applications with
	ImtCore, providing:
	- Backup configuration and settings management
	- Scheduled task parameters and configuration
	- Application lifecycle support
	- Settings persistence and validation
	
	The module integrates with:
	- ACF component framework for dependency injection
	- imtfile for file path management
	- Serialization infrastructure for settings persistence
	
	\section imtapp_architecture Architecture
	
	\subsection imtapp_patterns Design Patterns
	
	<b>Data Transfer Object (DTO) Pattern:</b>
	- ISchedulerParams and IBackupSettings as pure data containers
	- Separate interface from implementation
	- Facilitates serialization and persistence
	- Enables data validation and transformation
	
	<b>Component Pattern (ACF):</b>
	- CSchedulerParamsComp and CBackupSettingsComp as ACF components
	- Attribute-based configuration (I_ASSIGN macros)
	- Serialization through iser::ISerializable
	- Change notification through istd::IChangeable
	
	<b>Composite Pattern:</b>
	- IBackupSettings extends ISchedulerParams and IFileNameParam
	- Combines scheduling configuration with file location
	- Reuses common scheduling infrastructure
	- Composition over inheritance
	
	\subsection imtapp_core_interfaces Core Interfaces
	
	The module provides application framework interfaces:
	
	<b>Scheduler Configuration:</b>
	\code
	ISchedulerParams (iser::IObject)
	├─ GetStartTime() / SetStartTime() - Scheduled task start time
	├─ GetInterval() / SetInterval() - Task execution interval (seconds)
	└─ Extends iser::IObject for serialization
	    │
	    ├─ CSchedulerParams (data object implementation)
	    └─ CSchedulerParamsComp (ACF component)
	        ├─ Attributes: "StartTime", "Interval"
	        ├─ Implements iser::ISerializable
	        └─ Implements istd::IChangeable
	\endcode
	
	<b>Backup Configuration:</b>
	\code
	IBackupSettings (ISchedulerParams, ifile::IFileNameParam)
	├─ Inherits scheduling from ISchedulerParams
	├─ Inherits file path from IFileNameParam
	└─ Combines backup scheduling with target location
	    │
	    ├─ CBackupSettings (data object implementation)
	    └─ CBackupSettingsComp (ACF component)
	        ├─ Inherits scheduler attributes
	        ├─ Adds file path configuration
	        └─ Serializable backup configuration
	\endcode
	
	\section imtapp_components Core Components
	
	\subsection imtapp_scheduler Scheduler Parameters
	
	**ISchedulerParams / CSchedulerParamsComp** - Scheduled task configuration:
	
	\code{.cpp}
	// Create scheduler parameters
	auto scheduler = CSchedulerParamsComp::CreateInstance();
	
	// Configure start time
	QDateTime startTime = QDateTime::currentDateTime().addSecs(3600); // 1 hour from now
	scheduler->SetStartTime(startTime);
	
	// Configure interval (in seconds)
	int intervalInSeconds = 3600; // Run every hour
	scheduler->SetInterval(intervalInSeconds);
	
	// Access configuration
	QDateTime scheduledTime = scheduler->GetStartTime();
	int interval = scheduler->GetInterval();
	qDebug() << "Next run:" << scheduledTime 
	         << "Interval:" << interval << "seconds";
	\endcode
	
	**Component Attributes:**
	- `StartTime` (QString) - Formatted as "dd-MM-yyyy HH:mm"
	- `Interval` (int) - Interval in seconds between executions
	
	**Serialization:**
	- Implements iser::ISerializable for persistence
	- Can be saved to/loaded from XML, JSON, binary formats
	- Integrates with ACF serialization infrastructure
	
	\subsection imtapp_backup Backup Settings
	
	**IBackupSettings / CBackupSettingsComp** - Backup configuration:
	
	\code{.cpp}
	// Create backup settings
	auto backupSettings = CBackupSettingsComp::CreateInstance();
	
	// Configure backup schedule (inherited from ISchedulerParams)
	backupSettings->SetStartTime(QDateTime::currentDateTime());
	backupSettings->SetInterval(86400); // Daily backups (24 hours)
	
	// Configure backup target location (from IFileNameParam)
	QString backupPath = QStandardPaths::writableLocation(
	    QStandardPaths::AppDataLocation) + "/backups";
	backupSettings->SetFileName(backupPath);
	
	// Access complete configuration
	qDebug() << "Backup location:" << backupSettings->GetFileName();
	qDebug() << "Backup time:" << backupSettings->GetStartTime();
	qDebug() << "Backup interval:" << backupSettings->GetInterval() << "seconds";
	\endcode
	
	**Use Cases:**
	- Configuring automatic backup schedules
	- Persisting backup preferences
	- User-configurable backup settings UI
	- Integration with backup management systems
	
	\section imtapp_integration Integration Patterns
	
	\subsection imtapp_with_scheduler Integration with Task Scheduler
	
	**Pattern: Scheduled Backup Service:**
	\code{.cpp}
	class CBackupServiceComp : public ACF_COMPONENT(IBackupService)
	{
	    I_REFERENCE(IBackupSettings, m_backupSettings)
	    I_REFERENCE(IFileCompression, m_compression)
	    
	    QTimer* m_backupTimer;
	    
	public:
	    void InitializeComponent() override
	    {
	        // Create timer for scheduled backups
	        m_backupTimer = new QTimer(this);
	        connect(m_backupTimer, &QTimer::timeout, 
	                this, &CBackupServiceComp::PerformBackup);
	        
	        // Configure timer from settings
	        ConfigureScheduler();
	    }
	    
	    void ConfigureScheduler()
	    {
	        // Get interval from settings (in seconds)
	        int intervalSeconds = m_backupSettings->GetInterval();
	        m_backupTimer->setInterval(intervalSeconds * 1000); // Convert to ms
	        
	        // Calculate time until first backup
	        QDateTime startTime = m_backupSettings->GetStartTime();
	        QDateTime now = QDateTime::currentDateTime();
	        
	        if (startTime > now) {
	            // Delay first backup
	            int delayMs = now.msecsTo(startTime);
	            QTimer::singleShot(delayMs, this, &CBackupServiceComp::StartScheduler);
	        } else {
	            // Start immediately
	            StartScheduler();
	        }
	    }
	    
	    void StartScheduler()
	    {
	        m_backupTimer->start();
	        PerformBackup(); // Perform initial backup
	    }
	    
	    void PerformBackup()
	    {
	        QString backupPath = m_backupSettings->GetFileName();
	        QString dataDir = GetApplicationDataDirectory();
	        
	        // Create timestamped backup
	        QString timestamp = QDateTime::currentDateTime()
	            .toString("yyyyMMdd_hhmmss");
	        QString archivePath = QString("%1/backup_%2.zip")
	            .arg(backupPath)
	            .arg(timestamp);
	        
	        // Perform compression
	        m_compression->CompressFolder(dataDir, archivePath, true);
	        
	        qDebug() << "Backup completed:" << archivePath;
	    }
	};
	\endcode
	
	\subsection imtapp_with_settings Integration with Application Settings
	
	**Pattern: Persisted Configuration:**
	\code{.cpp}
	class CApplicationSettingsComp : public ACF_COMPONENT(IApplicationSettings)
	{
	    I_REFERENCE(IBackupSettings, m_backupSettings)
	    I_REFERENCE(IFilePersistence, m_persistence)
	    
	public:
	    bool LoadSettings()
	    {
	        // Load settings from file
	        QString settingsPath = GetSettingsFilePath();
	        
	        if (QFile::exists(settingsPath)) {
	            int result = m_persistence->LoadFromFile(
	                m_backupSettings, 
	                settingsPath);
	            
	            return result == ifile::IFilePersistence::OS_OK;
	        }
	        
	        // Use defaults if file doesn't exist
	        SetDefaultSettings();
	        return true;
	    }
	    
	    bool SaveSettings()
	    {
	        QString settingsPath = GetSettingsFilePath();
	        
	        int result = m_persistence->SaveToFile(
	            m_backupSettings, 
	            settingsPath);
	        
	        return result == ifile::IFilePersistence::OS_OK;
	    }
	    
	    void SetDefaultSettings()
	    {
	        // Set default backup schedule: daily at 2:00 AM
	        QDateTime nextBackup = QDateTime::currentDateTime();
	        nextBackup.setTime(QTime(2, 0));
	        if (nextBackup < QDateTime::currentDateTime()) {
	            nextBackup = nextBackup.addDays(1);
	        }
	        
	        m_backupSettings->SetStartTime(nextBackup);
	        m_backupSettings->SetInterval(86400); // 24 hours
	        
	        // Set default backup location
	        QString defaultPath = QStandardPaths::writableLocation(
	            QStandardPaths::AppDataLocation) + "/backups";
	        m_backupSettings->SetFileName(defaultPath);
	    }
	    
	    QString GetSettingsFilePath() const
	    {
	        return QStandardPaths::writableLocation(
	            QStandardPaths::AppConfigLocation) + "/backup_settings.xml";
	    }
	};
	\endcode
	
	\subsection imtapp_with_ui Integration with Settings UI
	
	**Pattern: Settings Dialog:**
	\code{.cpp}
	class CBackupSettingsDialogComp : public ACF_COMPONENT(ISettingsDialog)
	{
	    I_REFERENCE(IBackupSettings, m_backupSettings)
	    
	    QDateTimeEdit* m_startTimeEdit;
	    QSpinBox* m_intervalSpinBox;
	    QLineEdit* m_pathEdit;
	    QPushButton* m_browseButton;
	    
	public:
	    void InitializeUI()
	    {
	        // Create UI elements
	        m_startTimeEdit = new QDateTimeEdit();
	        m_startTimeEdit->setDisplayFormat("dd-MM-yyyy HH:mm");
	        
	        m_intervalSpinBox = new QSpinBox();
	        m_intervalSpinBox->setRange(60, 604800); // 1 min to 1 week
	        m_intervalSpinBox->setSuffix(" seconds");
	        
	        m_pathEdit = new QLineEdit();
	        m_browseButton = new QPushButton("Browse...");
	        
	        connect(m_browseButton, &QPushButton::clicked,
	                this, &CBackupSettingsDialogComp::OnBrowse);
	        
	        // Load current settings
	        LoadFromSettings();
	    }
	    
	    void LoadFromSettings()
	    {
	        m_startTimeEdit->setDateTime(m_backupSettings->GetStartTime());
	        m_intervalSpinBox->setValue(m_backupSettings->GetInterval());
	        m_pathEdit->setText(m_backupSettings->GetFileName());
	    }
	    
	    void SaveToSettings()
	    {
	        m_backupSettings->SetStartTime(m_startTimeEdit->dateTime());
	        m_backupSettings->SetInterval(m_intervalSpinBox->value());
	        m_backupSettings->SetFileName(m_pathEdit->text());
	    }
	    
	    void OnBrowse()
	    {
	        QString dir = QFileDialog::getExistingDirectory(
	            nullptr,
	            "Select Backup Directory",
	            m_pathEdit->text());
	        
	        if (!dir.isEmpty()) {
	            m_pathEdit->setText(dir);
	        }
	    }
	};
	\endcode
	
	\section imtapp_examples Complete Examples
	
	\subsection imtapp_example_service Complete Backup Service
	
	\code{.cpp}
	// Full-featured backup service with scheduling
	class CAutoBackupServiceComp : public ACF_COMPONENT(IAutoBackupService)
	{
	    I_REFERENCE(IBackupSettings, m_backupSettings)
	    I_REFERENCE(IFileCompression, m_compression)
	    I_REFERENCE(IFilePersistence, m_settingsPersistence)
	    
	    QTimer* m_schedulerTimer;
	    bool m_isRunning;
	    
	public:
	    CAutoBackupServiceComp() 
	        : m_schedulerTimer(nullptr)
	        , m_isRunning(false)
	    {
	    }
	    
	    void InitializeComponent() override
	    {
	        // Load settings
	        LoadSettings();
	        
	        // Create scheduler timer
	        m_schedulerTimer = new QTimer(this);
	        m_schedulerTimer->setSingleShot(false);
	        
	        connect(m_schedulerTimer, &QTimer::timeout,
	                this, &CAutoBackupServiceComp::OnScheduledBackup);
	        
	        // Monitor settings changes
	        connect(m_backupSettings, &IBackupSettings::Changed,
	                this, &CAutoBackupServiceComp::OnSettingsChanged);
	    }
	    
	    void Start()
	    {
	        if (m_isRunning) {
	            return;
	        }
	        
	        m_isRunning = true;
	        
	        // Configure and start scheduler
	        int intervalMs = m_backupSettings->GetInterval() * 1000;
	        m_schedulerTimer->setInterval(intervalMs);
	        
	        // Calculate time until first backup
	        QDateTime startTime = m_backupSettings->GetStartTime();
	        QDateTime now = QDateTime::currentDateTime();
	        
	        if (startTime > now) {
	            // Schedule first backup
	            int delayMs = now.msecsTo(startTime);
	            QTimer::singleShot(delayMs, [this]() {
	                OnScheduledBackup();
	                m_schedulerTimer->start();
	            });
	        } else {
	            // Start immediately
	            m_schedulerTimer->start();
	            OnScheduledBackup();
	        }
	        
	        qDebug() << "Backup service started";
	    }
	    
	    void Stop()
	    {
	        if (!m_isRunning) {
	            return;
	        }
	        
	        m_schedulerTimer->stop();
	        m_isRunning = false;
	        
	        qDebug() << "Backup service stopped";
	    }
	    
	    void OnScheduledBackup()
	    {
	        try {
	            QString backupDir = m_backupSettings->GetFileName();
	            
	            // Ensure backup directory exists
	            QDir().mkpath(backupDir);
	            
	            // Create timestamped archive name
	            QString timestamp = QDateTime::currentDateTime()
	                .toString("yyyyMMdd_hhmmss");
	            QString archivePath = QString("%1/backup_%2.zip")
	                .arg(backupDir)
	                .arg(timestamp);
	            
	            // Get application data directory
	            QString dataDir = QStandardPaths::writableLocation(
	                QStandardPaths::AppDataLocation) + "/data";
	            
	            // Perform backup
	            qDebug() << "Starting backup:" << archivePath;
	            
	            bool success = m_compression->CompressFolder(
	                dataDir,
	                archivePath,
	                true  // recursive
	            );
	            
	            if (success) {
	                qDebug() << "Backup completed successfully:" << archivePath;
	                CleanupOldBackups(backupDir);
	            } else {
	                qWarning() << "Backup failed:" << archivePath;
	            }
	            
	        } catch (const std::exception& e) {
	            qCritical() << "Backup error:" << e.what();
	        }
	    }
	    
	    void OnSettingsChanged()
	    {
	        // Save updated settings
	        SaveSettings();
	        
	        // Restart scheduler with new settings
	        if (m_isRunning) {
	            Stop();
	            Start();
	        }
	    }
	    
	    void LoadSettings()
	    {
	        QString settingsPath = GetSettingsFilePath();
	        
	        if (QFile::exists(settingsPath)) {
	            m_settingsPersistence->LoadFromFile(m_backupSettings, settingsPath);
	        } else {
	            // Set defaults
	            SetDefaultSettings();
	            SaveSettings();
	        }
	    }
	    
	    void SaveSettings()
	    {
	        QString settingsPath = GetSettingsFilePath();
	        m_settingsPersistence->SaveToFile(m_backupSettings, settingsPath);
	    }
	    
	    void SetDefaultSettings()
	    {
	        // Daily backups at 2:00 AM
	        QDateTime nextBackup = QDateTime::currentDateTime();
	        nextBackup.setTime(QTime(2, 0, 0));
	        if (nextBackup < QDateTime::currentDateTime()) {
	            nextBackup = nextBackup.addDays(1);
	        }
	        
	        m_backupSettings->SetStartTime(nextBackup);
	        m_backupSettings->SetInterval(86400); // 24 hours
	        
	        QString defaultPath = QStandardPaths::writableLocation(
	            QStandardPaths::AppDataLocation) + "/backups";
	        m_backupSettings->SetFileName(defaultPath);
	    }
	    
	    void CleanupOldBackups(const QString& backupDir, int keepCount = 10)
	    {
	        // Keep only the last N backups
	        QDir dir(backupDir);
	        QStringList backups = dir.entryList(
	            QStringList() << "backup_*.zip",
	            QDir::Files,
	            QDir::Time | QDir::Reversed);
	        
	        // Remove old backups
	        for (int i = keepCount; i < backups.size(); ++i) {
	            QString oldBackup = dir.filePath(backups[i]);
	            QFile::remove(oldBackup);
	            qDebug() << "Removed old backup:" << oldBackup;
	        }
	    }
	    
	    QString GetSettingsFilePath() const
	    {
	        return QStandardPaths::writableLocation(
	            QStandardPaths::AppConfigLocation) + "/backup_settings.xml";
	    }
	};
	\endcode
	
	\section imtapp_best_practices Best Practices
	
	\subsection imtapp_validation Input Validation
	
	- Validate start times to ensure they're in the future or current
	- Enforce minimum and maximum intervals
	- Verify backup paths are writable before saving settings
	- Provide user feedback for invalid configurations
	
	\subsection imtapp_persistence Settings Persistence
	
	- Save settings immediately when changed (or provide "Apply" button)
	- Use appropriate serialization format (XML for human-readability)
	- Handle missing settings files gracefully with defaults
	- Backup settings before overwriting
	
	\subsection imtapp_scheduling Scheduling Considerations
	
	- Handle time zone changes appropriately
	- Consider daylight saving time transitions
	- Provide options for missed executions (run immediately or skip)
	- Log all scheduled task executions
	
	\section imtapp_related_modules Integration with Other Modules
	
	<b>With imtfile (File Operations):</b>
	- Uses IFileNameParam for file path configuration
	- Integration with file compression services
	- Backup file management
	
	<b>With imtdb (Database):</b>
	- Database backup scheduling
	- Configuration persistence in database
	- Transaction-based settings updates
	
	<b>With imtgui (UI Components):</b>
	- Settings dialog implementations
	- Visual scheduler configuration
	- Backup status displays
	
	\section imtapp_references References
	
	<b>Related Modules:</b>
	- ifile (ACF) - File name parameter interfaces
	- iser (ACF) - Serialization infrastructure
	- istd (ACF) - Change notification
	- imtfile - File operations and compression
	
	<b>ACF Interfaces:</b>
	- iser::ISerializable - Persistence interface
	- iser::IObject - Base serializable object
	- istd::IChangeable - Change notification
	- ifile::IFileNameParam - File path parameter
	
	<b>External Documentation:</b>
	- Qt QTimer: https://doc.qt.io/qt-6/qtimer.html
	- Qt QDateTime: https://doc.qt.io/qt-6/qdatetime.html
	- Qt QStandardPaths: https://doc.qt.io/qt-6/qstandardpaths.html
*/

namespace imtapp
{
	// Namespace for desktop application framework
}
