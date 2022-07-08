#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtdb/IDatabaseEngine.h>
#include <imtgql/IGqlRepresentationDataController.h>


namespace imtgql
{


class CGqlRepresentationDataControllerCompBase:
			public ilog::CLoggerComponentBase,
			virtual public imtgql::IGqlRepresentationDataController
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CGqlRepresentationDataControllerCompBase);
		I_REGISTER_INTERFACE(imtgql::IGqlRepresentationDataController);
		I_ASSIGN_MULTI_0(m_modelIdsCompPtr, "ModelIds", "List of model-IDs for GraphQL-response", true);
		I_ASSIGN_MULTI_0(m_contextIdsCompPtr, "ContextIds", "List of context-IDs for GraphQL-response", false);
        I_ASSIGN(m_databaseEngineCompPtr, "Database", "Database for working", false, "IDatabaseEngine");
	I_END_COMPONENT;

	// reimplemented (imtgql::IGqlRepresentationDataController)
	virtual QByteArrayList GetModelIds() const override;
	virtual QByteArrayList GetContextIds() const override;

protected:
	I_MULTIATTR(QByteArray, m_modelIdsCompPtr);
	I_MULTIATTR(QByteArray, m_contextIdsCompPtr);
	I_REF(imtdb::IDatabaseEngine, m_databaseEngineCompPtr);
};


} // namespace imtgql


