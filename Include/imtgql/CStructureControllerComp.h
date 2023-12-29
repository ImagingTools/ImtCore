#pragma once


// ImtCore includes
#include <imtgql/CStructureControllerCompBase.h>
#include <imtdb/IDatabaseEngine.h>


#undef GetObject


namespace imtgql
{


class CStructureControllerComp: public imtgql::CStructureControllerCompBase
{
public:
	typedef imtgql::CStructureControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CStructureControllerComp);
		// I_ASSIGN(m_objectCollectionCompPtr, "ObjectCollection", "Object collection", true, "ObjectCollection");
		I_ASSIGN(m_databaseEngineCompPtr, "DatabaseEngine", "Database engine for SQL queries", true, "DatabaseEngine");
	I_END_COMPONENT;

protected:
	QByteArray GetElementsQuery(iprm::IParamsSet* filterParams) const;

	// reimpemented (CStructureControllerCompBase)
	virtual imtbase::CTreeItemModel* GetElements(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	// virtual imtbase::CTreeItemModel* RemoveNode(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	// virtual imtbase::CTreeItemModel* Re"UserId"moveObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;


protected:
	// I_REF(imtbase::IObjectCollection, m_objectCollectionCompPtr);
	I_REF(imtdb::IDatabaseEngine, m_databaseEngineCompPtr);
};


} // namespace imtguigql


