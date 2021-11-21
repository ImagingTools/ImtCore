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
		I_ASSIGN(m_modelIdCompPtr, "ModelId", "Model Id", true, "");
		I_ASSIGN(m_databaseCompPtr, "Database", "Database for working", false, "IDataBaseEngine");
	I_END_COMPONENT;

// reimplemented (ilog::CLoggerComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

// reimplemented (imtgql::IGqlRepresentationDataController)
	virtual QByteArray GetModelId() const override;
	virtual imtbase::CTreeItemModel* CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:

	I_ATTR(QByteArray, m_modelIdCompPtr);
	I_REF(imtdb::IDatabaseEngine, m_databaseCompPtr);
};


} // namespace rtsgql
