#pragma once

// ACF includes
#include <ilog/CMessageContainer.h>

// ImtCore includes
#include <imtgql/CGqlRequestHandlerCompBase.h>
#include <imtclientgql/CApiClientComp.h>
#include <imtgql/IGqlResponse.h>


namespace imtclientgql
{


class CGqlRemoteRepresentationControllerCompBase: public imtgql::CGqlRequestHandlerCompBase
{
public:
	typedef imtgql::CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlRemoteRepresentationControllerCompBase)
		I_ASSIGN(m_apiClientCompPtr, "ApiClient", "Api Client", true, "ApiClient");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CGqlRepresentationDataControllerComp)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	imtbase::CTreeItemModel* CreateTreeItemModelFromResponse(const imtgql::IGqlResponse& response);

protected:
	I_REF(imtclientgql::IGqlClient, m_apiClientCompPtr);
};


} // namespace imtclientgql


