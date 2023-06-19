#pragma once


// ImtCore includes
#include <imtgql/CGqlRequestHandlerCompBase.h>
#include <imtgql/IGqlContextCacheController.h>


namespace imtauthgql
{


class CLogoutControllerComp: public imtgql::CGqlRequestHandlerCompBase
{
public:
	typedef imtgql::CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CLogoutControllerComp);
		I_ASSIGN(m_gqlContextCacheControllerCompPtr, "GqlContextCacheController", "Gql context cache controller", true, "GqlContextCacheController");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CGqlRepresentationControllerCompBase)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_REF(imtgql::IGqlContextCacheController, m_gqlContextCacheControllerCompPtr);
};


} // namespace imtauthgql


