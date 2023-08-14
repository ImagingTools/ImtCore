#pragma once

// ACF includes
#include <ilog/CMessageContainer.h>

// ImtCore includes
#include <imtgql/CGqlRequestHandlerCompBase.h>
#include <imtgql/CApiClientComp.h>
#include <imtgql/IGqlResponse.h>


namespace imtgql
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

protected:
	class Response: public ilog::CMessageContainer, virtual public IGqlResponse
	{
	public:
		Response();
		virtual imtbase::CTreeItemModel* GetResult();

		// reimplemented (imtgql::IGqlClient::ResponseHandler)
		virtual void OnReply(const IGqlRequest& request, const QByteArray& replyData) override;

		virtual bool IsSuccessfull() const override { return false; }

	private:
		imtbase::CTreeItemModel* m_replyResultPtr;
	};

protected:
	I_REF(imtgql::IGqlClient, m_apiClientCompPtr);
};


} // namespace imtgql


