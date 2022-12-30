#pragma once


// ImtCore includes
#include <imtgql/CGqlRepresentationDataControllerComp.h>
#include <imtgql/CApiClientComp.h>


namespace imtgql
{


class CGqlRemoteRepresentationControllerComp:
		public imtgql::CGqlRepresentationDataControllerComp
{
public:
	typedef imtgql::CGqlRepresentationDataControllerComp BaseClass;

	I_BEGIN_COMPONENT(CGqlRemoteRepresentationControllerComp)
		I_ASSIGN(m_apiClientCompPtr, "ApiClient", "Api Client", true, "ApiClient");
	I_END_COMPONENT;

	class Response: virtual public imtgql::IGqlClient::ResponseHandler
	{
	public:
		virtual imtbase::CHierarchicalItemModelPtr GetResult();

		// reimplemented (imtgql::IGqlClient::ResponseHandler)
		virtual void OnReply(const IGqlRequest& request, const QByteArray& replyData) override;

	private:
		imtbase::CHierarchicalItemModelPtr m_replyResultPtr;
	};

protected:
	// reimplemented (imtgql::CGqlRepresentationDataControllerComp)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_REF(imtgql::IGqlClient, m_apiClientCompPtr);
};


} // namespace imtgql


