#pragma once


// ImtCore includes
#include <imtgql/CStructureControllerCompBase.h>
#include <imtdb/IDatabaseEngine.h>


namespace imtdbgql
{


class CStructureControllerComp: public imtgql::CStructureControllerCompBase
{
public:
	typedef imtgql::CStructureControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CStructureControllerComp);
		I_ASSIGN(m_databaseEngineCompPtr, "DatabaseEngine", "Database engine for SQL queries", true, "DatabaseEngine");
	I_END_COMPONENT;

protected:
	QByteArray GetElementsQuery(iprm::IParamsSet* filterParams) const;

	// reimpemented (CStructureControllerCompBase)
	virtual imtbase::CTreeItemModel* GetElements(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;

protected:
	I_REF(imtdb::IDatabaseEngine, m_databaseEngineCompPtr);
};


} // namespace imtdbgql


