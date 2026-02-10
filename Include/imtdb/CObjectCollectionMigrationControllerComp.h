// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtdb/CMigrationControllerCompBase.h>
#include <imtdb/ISqlDatabaseObjectDelegate.h>


namespace imtdb
{


class CObjectCollectionMigrationControllerComp: virtual public imtdb::CMigrationControllerCompBase
{
public:
	typedef imtdb::CMigrationControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CObjectCollectionMigrationControllerComp);
		I_ASSIGN_MULTI_0(m_sqlDatabaseObjectDelegatesCompPtr, "SqlDatabaseObjectDelegates", "List of object collection sql delegates", false);
	I_END_COMPONENT;

protected:
	// reimplemented (imtdb::IMigrationController)
	virtual bool DoMigration(int& resultRevision, const istd::CIntRange& subRange = istd::CIntRange()) const override;

private:
	I_MULTIREF(imtdb::ISqlDatabaseObjectDelegate, m_sqlDatabaseObjectDelegatesCompPtr);
};


} // namespace imtdb


