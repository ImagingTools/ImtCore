// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtdb/IDatabaseEngine.h>
#include <imtservergql/CStructureControllerCompBase.h>


namespace imtdbgql
{


class CStructureControllerComp: public imtservergql::CStructureControllerCompBase
{
public:
	typedef imtservergql::CStructureControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CStructureControllerComp);
		I_ASSIGN(m_databaseEngineCompPtr, "DatabaseEngine", "Database engine for SQL queries", true, "DatabaseEngine");
	I_END_COMPONENT;

protected:
	QByteArray GetElementsQuery(iprm::IParamsSet* filterParams) const;

	// reimpemented (CStructureControllerCompBase)
	virtual imtbase::CTreeItemModel* GetElements(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_REF(imtdb::IDatabaseEngine, m_databaseEngineCompPtr);
};


} // namespace imtdbgql


