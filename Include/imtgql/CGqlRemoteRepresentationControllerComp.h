#pragma once


// ImtCore includes
#include <imtgql/CGqlRequestHandlerCompBase.h>
#include <imtgql/CApiClientComp.h>


namespace imtgql
{


class CGqlRemoteRepresentationControllerComp: public imtgql::CGqlRequestHandlerCompBase
{
public:
	typedef imtgql::CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlRemoteRepresentationControllerComp)
		I_ASSIGN(m_apiClientCompPtr, "ApiClient", "Api Client", true, "ApiClient");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CGqlRepresentationDataControllerComp)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

protected:
	class Response: virtual public imtgql::IGqlClient::ResponseHandler
	{
	public:
		Response();
		virtual imtbase::CTreeItemModel* GetResult();

		// reimplemented (imtgql::IGqlClient::ResponseHandler)
		virtual void OnReply(const IGqlRequest& request, const QByteArray& replyData) override;

	private:
		imtbase::CTreeItemModel* m_replyResultPtr;
	};
protected:
	I_REF(imtgql::IGqlClient, m_apiClientCompPtr);
};


} // namespace imtgql


