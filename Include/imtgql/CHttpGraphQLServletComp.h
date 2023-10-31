#pragma once


// ImtCore includes
#include <imtrest/IProtocolEngine.h>
#include <imtrest/CHttpServletCompBase.h>
#include <imtgql/IGqlRequestHandler.h>
#include <imtgql/IGqlContextController.h>
#include <imtgql/IGqlRequestProvider.h>


namespace imtgql
{


class CHttpGraphQLServletComp: public imtrest::CHttpServletCompBase, virtual public imtgql::IGqlRequestProvider
{
public:
	typedef imtrest::CHttpServletCompBase BaseClass;

	I_BEGIN_COMPONENT(CHttpGraphQLServletComp);
		I_REGISTER_INTERFACE(imtgql::IGqlRequestProvider);
		I_ASSIGN_MULTI_0(m_gqlRequestHandlerCompPtr, "GqlRequestHandler", "Handler for GraphQL-request", true);
		I_ASSIGN(m_gqlContextControllerCompPtr, "GqlContextController", "GraphQL-related context controller", false, "GqlContextController");
		I_ASSIGN(m_userSettingsCollectionCompPtr, "UserSettingsCollection", "User settings collection", false, "UserSettingsCollection");
	I_END_COMPONENT;

protected:
	// reimplemented (imtrest::CHttpServletCompBase)
	virtual imtrest::ConstResponsePtr OnPost(const QByteArray& commandId, const imtrest::IRequest::CommandParams& commandParams, const HeadersMap& headers, const imtrest::CHttpRequest& request) const override;

	// reimplemented (imtgql::IGqlRequestProvider)
	virtual const IGqlRequest* GetGqlRequest() const override;
private:
	imtrest::ConstResponsePtr CreateResponse(
				const imtrest::IProtocolEngine::StatusCode& statusCode,
				const QByteArray& payload,
				const imtrest::IRequest& request,
				const QByteArray& contentTypeId = "text/plain; charset=utf-8") const;
	imtrest::ConstResponsePtr GenerateError(
				const imtrest::IProtocolEngine::StatusCode& errorCode,
				const QString& errorString,
				const imtrest::CHttpRequest& request) const;

private:
	I_MULTIREF(imtgql::IGqlRequestHandler, m_gqlRequestHandlerCompPtr);
	I_REF(imtgql::IGqlContextController, m_gqlContextControllerCompPtr);
	I_REF(imtbase::IObjectCollection, m_userSettingsCollectionCompPtr);

private:
	mutable imtgql::CGqlRequest m_gqlRequest;
};


} // namespace imtgql


