// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QProcess>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtrest/IBinaryDataController.h>
#include <imtrest/IBinaryDataProvider.h>
#include <imtdb/IDatabaseLoginSettings.h>


namespace imtdb
{


class CRestoringDatabaseControllerComp:
			public ilog::CLoggerComponentBase,
			public imtrest::IBinaryDataController,
			public imtrest::IBinaryDataProvider

{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CRestoringDatabaseControllerComp);
		I_REGISTER_INTERFACE(IBinaryDataController);
		I_REGISTER_INTERFACE(IBinaryDataProvider);
		I_ASSIGN(m_databaseLoginSettingsCompPtr, "DatabaseLoginSettings", "Database login settings", true, "DatabaseLoginSettings");
		I_ASSIGN(m_backupSettingsCompPtr, "BackupSettings", "Backup settings", false, "BackupSettings");
		I_ASSIGN(m_backupBeforeRestoreAttrPtr, "BackupBeforeRestore", "Backup before restore", false, true);
		I_ASSIGN(m_commandIdAttrPtr, "Command-ID", "Command ID", true, "");
	I_END_COMPONENT;

	// reimplemented (imtrest::IBinaryDataController)
	virtual bool SetData(const QByteArray& data, QByteArray& dataId) const override;
	virtual bool RemoveData(const QByteArray& dataId) const override;
	virtual bool EnsureDataExists(const QByteArray& dataId) const override;

	// reimplemented (imtrest::IBinaryDataProvider)
	virtual bool GetData(
				QByteArray& data,
				const QByteArray& dataId,
				qint64 readFromPosition = 0,
				qint64 readMaxLength = -1) const override;

protected:
	 bool ExecuteCommand(QProcess& process, const QString& command) const;

protected:
	I_REF(imtdb::IDatabaseLoginSettings, m_databaseLoginSettingsCompPtr);
	I_REF(iprm::IParamsSet, m_backupSettingsCompPtr);
	I_ATTR(bool, m_backupBeforeRestoreAttrPtr);
	I_ATTR(QByteArray, m_commandIdAttrPtr);
};


} // namespace imtdb


