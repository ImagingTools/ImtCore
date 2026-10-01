// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CGqlRequestHandlerCompBase.h>


// ImtCore includes
#include <imtbase/CTenantSecurityContextScope.h>


namespace imtservergql
{


// public methods

// reimplemented (imtgql::IGqlRequestHandler)

bool CGqlRequestHandlerCompBase::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	QByteArray commandId = gqlRequest.GetCommandId();

	return m_commandIdsAttrPtr.FindValue(commandId) != -1;
}


QJsonObject CGqlRequestHandlerCompBase::CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	Q_ASSERT(IsRequestSupported(gqlRequest));

	if (!IsRequestSupported(gqlRequest)){
		SendErrorMessage(0, QStringLiteral("GQL handler is not supported GQL request with command-ID:'%1'").arg(gqlRequest.GetCommandId()));

		return QJsonObject();
	}

	// Requests without context (e.g. internal nested requests) keep the security context of the caller.
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		return CreateInternalResponse(gqlRequest, errorMessage);
	}

	// All database queries of this request are restricted to the tenant of the request (see imtdb::CTenantRowLevelSecurityControllerComp)
	imtbase::CTenantSecurityContextScope tenantContextScope(imtbase::CTenantSecurityContext(gqlContextPtr->GetTenantId(), gqlContextPtr->GetUserId()));

	return CreateInternalResponse(gqlRequest, errorMessage);
}


// protected methods

iprm::IParamsSetUniquePtr CGqlRequestHandlerCompBase::CreateContextParams(const imtgql::CGqlRequest& /*gqlRequest*/) const
{
	return nullptr;
}


} // namespace imtservergql


