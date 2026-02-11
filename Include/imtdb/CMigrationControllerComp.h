// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtdb/CMigrationControllerCompBase.h>


namespace imtdb
{


class CMigrationControllerComp: virtual public imtdb::CMigrationControllerCompBase
{
public:
	typedef imtdb::CMigrationControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CMigrationControllerComp);
		I_ASSIGN(m_migrationFolderPathCompPtr, "MigrationFolderPath", "The property holds the folder contains SQL migraton script", false, "MigrationFolderPath");
	I_END_COMPONENT;

protected:
	// reimplemented (imtdb::IMigrationController)
	virtual bool DoMigration(int& resultRevision, const istd::CIntRange& subRange = istd::CIntRange()) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	int GetLastMigration(const QString& migrationFolder, QString& errorMessage) const;

protected:
	I_REF(ifile::IFileNameParam, m_migrationFolderPathCompPtr);
};


} // namespace imtdb


