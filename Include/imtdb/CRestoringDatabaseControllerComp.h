#pragma once


// Qt includes
#include <QtCore/QProcess>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtbase/IBinaryDataController.h>
#include <imtbase/IBinaryDataProvider.h>
#include <imtdb/IDatabaseLoginSettings.h>


namespace imtdb
{


class CRestoringDatabaseControllerComp:
			public ilog::CLoggerComponentBase,
			public imtbase::IBinaryDataController,
			public imtbase::IBinaryDataProvider

{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CRestoringDatabaseControllerComp);
		I_REGISTER_INTERFACE(IBinaryDataController);
		I_REGISTER_INTERFACE(IBinaryDataProvider);
		I_ASSIGN(m_databaseLoginSettingsCompPtr, "DatabaseLoginSettings", "Database login settings", true, "DatabaseLoginSettings");
		I_ASSIGN(m_backupSettingsCompPtr, "BackupSettings", "Backup settings", false, "BackupSettings");
		I_ASSIGN(m_backupBeforeRestoreAttrPtr, "BackupBeforeRestore", "Backup before restore", false, true);
	I_END_COMPONENT;

	// reimplemented (imtbase::IBinaryDataController)
	virtual bool SetData(const QByteArray& data, QByteArray& dataId) const override;
	virtual bool RemoveData(const QByteArray& dataId) const override;
	virtual bool EnsureDataExists(const QByteArray& dataId) const override;

	// reimplemented (imtbase::IBinaryDataProvider)
	virtual bool GetData(QByteArray& data, const QByteArray& dataId) const override;

protected:
	 bool ExecuteCommand(QProcess& process, const QString& command) const;

protected:
	I_REF(imtdb::IDatabaseLoginSettings, m_databaseLoginSettingsCompPtr);
	I_REF(iprm::IParamsSet, m_backupSettingsCompPtr);
	I_ATTR(bool, m_backupBeforeRestoreAttrPtr);
};


} // namespace imtdb


