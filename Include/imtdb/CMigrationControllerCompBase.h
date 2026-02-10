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


class CMigrationControllerCompBase: public ilog::CLoggerComponentBase, virtual public imtdb::IMigrationController
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CMigrationControllerCompBase);
		I_REGISTER_INTERFACE(imtdb::IMigrationController)
		I_ASSIGN(m_rangeFromAttrPtr, "From", "Range of from value", false, -1);
		I_ASSIGN(m_rangeToAttrPtr, "To", "if value -1, then maximum value in the migration folder", false, -1);
		I_ASSIGN(m_databaseEngineCompPtr, "DatabaseEngine", "Database engine", true, "DatabaseEngine");
	I_END_COMPONENT;

protected:
	// reimplemented (imtdb::IMigrationController)
	virtual istd::CIntRange GetMigrationRange() const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

protected:
	I_REF(imtdb::IDatabaseEngine, m_databaseEngineCompPtr);
	I_ATTR(int, m_rangeFromAttrPtr);
	I_ATTR(int, m_rangeToAttrPtr);

	istd::CIntRange m_range;
};


} // namespace imtdb


