// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include "QtCore/QString"

// ACF includes
#include <icomp/CComponentBase.h>
#include <istd/TDelPtr.h>
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtmdbx/CMdbxDatabaseEngine.h>


namespace imtmdbx
{


class CMdbxDatabaseEngineComp: public icomp::CComponentBase, virtual public IMdbxDatabaseEngine
{

public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CMdbxDatabaseEngineComp);
	I_REGISTER_INTERFACE(IMdbxDatabaseEngine);
		I_ASSIGN(m_dbPathCompPtr, "DbPath", "The property holds connection's database path", true, "");
	I_END_COMPONENT;

	// reimplemented (IMdbxDatabaseEngine)
	virtual mdbx::env_managed& GetEnv() override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	istd::TDelPtr<CMdbxDatabaseEngine> m_databaseEngine;

	I_REF(ifile::IFileNameParam, m_dbPathCompPtr);
	// I_ATTR(QByteArray, m_dbPathAttrPtr);
};


}//namespace imtmdbx
