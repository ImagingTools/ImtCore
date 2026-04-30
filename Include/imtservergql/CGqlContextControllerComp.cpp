// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CGqlContextControllerComp.h>


// ImtCore includes
#include <imtbase/imtbase.h>


namespace imtservergql
{


// public methods

// reimplemented (imtgql::IGqlContextController)

imtgql::IGqlContext* CGqlContextControllerComp::GetRequestContext(
			const imtgql::CGqlRequest& /*gqlRequest*/,
			const QByteArray& token,
			const imtgql::IGqlContext::Headers& headers,
			QString& errorMessage) const
{
	if (!m_gqlContextCreatorCompPtr.IsValid()){
		errorMessage = QStringLiteral("GraphQL context creator is not configured.");
		SendErrorMessage(0, errorMessage, "CGqlContextControllerComp");
		return nullptr;
	}

	imtgql::IGqlContext::Headers contextHeaders = headers;
	// Preserve legacy callers that still pass the auth token separately.
	if (!token.isEmpty() && !contextHeaders.contains(imtbase::s_authenticationTokenHeaderId)){
		contextHeaders.insert(imtbase::s_authenticationTokenHeaderId, token);
	}

	imtgql::IGqlContextCreator::ContextCreationError contextError;
	imtgql::IGqlContextUniquePtr gqlContextPtr = m_gqlContextCreatorCompPtr->CreateGqlContext(contextHeaders, contextError);
	if (!gqlContextPtr.IsValid()){
		errorMessage = contextError.message;
		SendErrorMessage(0, errorMessage, "CGqlContextControllerComp");
		return nullptr;
	}

	return gqlContextPtr.PopInterfacePtr();
}


} // namespace imtservergql
