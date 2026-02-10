// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/ITransactionManager.h>
#include <imtdb/ISqlDatabaseObjectCollection.h>
#include <imtdb/IDatabaseEngine.h>


namespace imtdb
{


class CSqlDatabaseTransactionManagerComp: public icomp::CComponentBase, virtual public imtbase::ITransactionManager
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSqlDatabaseTransactionManagerComp);
		I_REGISTER_INTERFACE(imtbase::ITransactionManager);
		I_ASSIGN_MULTI_0(m_collectionCompPtr, "SqlDatabaseObjectCollection", "Sql database collections to disable their own transaction mechanism", false);
		I_ASSIGN(m_databaseEngineCompPtr, "DatabaseEngine", "Database engine", false, "DatabaseEngine");
	I_END_COMPONENT;

	// reimplemented (imtbase::ITransactionManager)
	virtual bool StartTransaction() override;
	virtual bool CancelTransaction() override;
	virtual bool EndTransaction() override;

private:
	bool IsCollectionsInOneDatabase() const;

private:
	I_MULTIREF(imtdb::ISqlDatabaseObjectCollection, m_collectionCompPtr);
	I_REF(imtdb::IDatabaseEngine, m_databaseEngineCompPtr);
};


} // namespace imtdb


