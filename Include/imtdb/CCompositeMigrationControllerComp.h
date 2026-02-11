// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtdb/IMigrationController.h>
#include <imtdb/IDatabaseEngine.h>


namespace imtdb
{


class CCompositeMigrationControllerComp: public ilog::CLoggerComponentBase, virtual public imtdb::IMigrationController
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCompositeMigrationControllerComp);
		I_REGISTER_INTERFACE(imtdb::IMigrationController)
		I_ASSIGN_MULTI_0(m_migrationControllersCompPtr, "MigrationControllers", "Migration controllers", false);
	I_END_COMPONENT;

protected:
	struct MigrationStep
	{
		int from = -1;
		int to = -1;
		const imtdb::IMigrationController* migrationControllerPtr = nullptr;
	};

	typedef QVector<MigrationStep> MigrationSteps;

	const imtdb::IMigrationController* FindMigrationController(int migrationIndex) const;

	// reimplemented (imtdb::IMigrationController)
	virtual istd::CIntRange GetMigrationRange() const override;
	virtual bool DoMigration(int& resultRevision, const istd::CIntRange& subRange = istd::CIntRange()) const override;
	
private:
	MigrationStep CreateMigrationStep(int from, int to, const imtdb::IMigrationController* migrationControllerPtr) const;
	bool ContainsStep(const istd::CIntRange& range, const MigrationSteps steps) const;

private:
	I_MULTIREF(imtdb::IMigrationController, m_migrationControllersCompPtr);
};


} // namespace imtdb


