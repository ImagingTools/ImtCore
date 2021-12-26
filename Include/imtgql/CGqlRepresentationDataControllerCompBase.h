#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtdb/IDataBaseEngine.h>
#include <imtgql/IGqlRepresentationDataController.h>


namespace imtgql
{


class CGqlRepresentationDataControllerCompBase: public ilog::CLoggerComponentBase, public imtgql::IGqlRepresentationDataController
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGqlRepresentationDataControllerCompBase);
		I_REGISTER_INTERFACE(imtgql::IGqlRepresentationDataController);
		I_ASSIGN_MULTI_0(m_modelIdsCompPtr, "ModelIds", "List of model-IDs for graphQl response", true);
		I_ASSIGN(m_databaseCompPtr, "Database", "Database for working", false, "IDataBaseEngine");
	I_END_COMPONENT;

	// reimplemented (imtgql::IGqlRepresentationDataController)
	virtual QByteArrayList GetModelIds() const override;
	virtual imtbase::CTreeItemModel* CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

protected:
	I_MULTIATTR(QByteArray, m_modelIdsCompPtr);
	I_REF(imtdb::IDatabaseEngine, m_databaseCompPtr);
};


} // namespace rtsgql
