#pragma once


// ImtCore includes
#include <imtrest/IProtocolEngine.h>
#include <imtrest/CHttpServletCompBase.h>
#include <imtgql/IGqlRequestHandler.h>
#include <imtgql/IGqlContextController.h>


namespace imtgql
{


class CHttpGraphQLServletComp: public imtrest::CHttpServletCompBase
{
public:
	typedef imtrest::CHttpServletCompBase BaseClass;

	I_BEGIN_COMPONENT(CHttpGraphQLServletComp);
		I_ASSIGN_MULTI_0(m_gqlRequestHandlerCompPtr, "GqlRequestHandler", "Handler for GraphQL-request", true);
		I_ASSIGN(m_gqlContextControllerCompPtr, "GqlContextController", "GraphQL-related context controller", false, "GqlContextController");
	I_END_COMPONENT;

protected:
	// reimplemented (imtrest::CHttpServletCompBase)
	virtual ConstResponsePtr OnPost(const QByteArray& commandId, const imtrest::IRequest::CommandParams& commandParams, const HeadersMap& headers, const imtrest::CHttpRequest& request) const override;

private:
	imtrest::IRequestServlet::ConstResponsePtr CreateResponse(
				const imtrest::IProtocolEngine::StatusCode& statusCode,
				const QByteArray& payload,
				const imtrest::IRequest& request,
				const QByteArray& contentTypeId = "text/plain; charset=utf-8") const;
	imtrest::IRequestServlet::ConstResponsePtr GenerateError(
				const imtrest::IProtocolEngine::StatusCode& errorCode,
				const QString& errorString,
				const imtrest::CHttpRequest& request) const;

private:
	I_MULTIREF(imtgql::IGqlRequestHandler, m_gqlRequestHandlerCompPtr);
	I_REF(imtgql::IGqlContextController, m_gqlContextControllerCompPtr);
};


} // namespace imtgql


