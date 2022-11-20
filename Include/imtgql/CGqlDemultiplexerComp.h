#pragma once


// ImtCore includes
#include <imtgql/CGqlRepresentationDataControllerCompBase.h>


namespace imtgql
{


class CGqlDemultiplexerComp:
		public ilog::CLoggerComponentBase,
		virtual public imtgql::IGqlRepresentationDataController
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGqlDemultiplexerComp);
		I_REGISTER_INTERFACE(imtgql::IGqlRepresentationDataController);
		I_ASSIGN_MULTI_0(m_gqlRepresentationDataControllerCompPtr, "GqlRepresentationDataController", "Gql representationDataController", true);
		I_ASSIGN_MULTI_0(m_contextIdsCompPtr, "GqlContextIds", "Gql context Ids", false);
	I_END_COMPONENT;

	// reimplemented (imtgql::IGqlRepresentationDataController)
	virtual imtbase::CTreeItemModel* CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual QByteArrayList GetModelIds() const override;
	virtual QByteArrayList GetContextIds() const override;

protected:
	I_MULTIREF(imtgql::IGqlRepresentationDataController, m_gqlRepresentationDataControllerCompPtr);
	I_MULTIATTR(QByteArray, m_contextIdsCompPtr);
};


} // namespace imtgql








