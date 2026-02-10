// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtrest/IProtocolEngine.h>
#include <imtrest/CHttpServletCompBase.h>
#include <imtgql/IGqlRequestHandler.h>
#include <imtgql/IGqlContextCreator.h>
#include <imtgql/IGqlRequestProvider.h>
#include <imtbase/IObjectCollection.h>
#include <imtauth/IJwtSessionController.h>
#include <imtauth/IPersonalAccessTokenManager.h>


namespace imtservergql
{


class CHttpGraphQLServletComp: public imtrest::CHttpServletCompBase, virtual public imtgql::IGqlRequestProvider
{
public:
	using BaseClass = imtrest::CHttpServletCompBase;
	using StatusCode = imtrest::IProtocolEngine::StatusCode;

	I_BEGIN_COMPONENT(CHttpGraphQLServletComp);
		I_REGISTER_INTERFACE(imtgql::IGqlRequestProvider);
		I_ASSIGN_MULTI_0(m_gqlRequestHandlerCompPtr, "GqlRequestHandler", "Handler for GraphQL-request", true);
		I_ASSIGN(m_gqlContextCreatorCompPtr, "GqlContextCreator", "GraphQL-related context creator", false, "GqlContextCreator");
		I_ASSIGN(m_jwtSessionControllerCompPtr, "JwtSessionController", "JWT session controller", false, "JwtSessionController");
		I_ASSIGN(m_patManagerCompPtr, "PersonalAccessTokenManager", "Personal Access Token manager", false, "PersonalAccessTokenManager");
	I_END_COMPONENT;

protected:
	// reimplemented (IRequestHandler)
	virtual bool IsCommandSupported(const QByteArray& commandId) const override;

	// reimplemented (imtrest::CHttpServletCompBase)
	virtual imtrest::ConstResponsePtr OnPost(const QByteArray& commandId, const imtrest::IRequest::CommandParams& commandParams, const HeadersMap& headers, const imtrest::CHttpRequest& request) const override;

	// reimplemented (imtgql::IGqlRequestProvider)
	virtual const imtgql::IGqlRequest* GetGqlRequest() const override;

private:
	imtrest::ConstResponsePtr CreateResponse(
				const StatusCode& statusCode,
				const QByteArray& payload,
				const imtrest::IRequest& request,
				const QByteArray& contentTypeId = "text/plain; charset=utf-8") const;
	imtrest::ConstResponsePtr GenerateError(
				const StatusCode& errorCode,
				const QString& errorString,
				const imtrest::CHttpRequest& request) const;

private:
	I_MULTIREF(imtgql::IGqlRequestHandler, m_gqlRequestHandlerCompPtr);
	I_REF(imtgql::IGqlContextCreator, m_gqlContextCreatorCompPtr);
	I_REF(imtauth::IJwtSessionController, m_jwtSessionControllerCompPtr);
	I_REF(imtauth::IPersonalAccessTokenManager, m_patManagerCompPtr);

private:
	mutable imtgql::CGqlRequest m_lastRequest;
};


} // namespace imtservergql


