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

	QByteArray productId = headers.value(imtbase::s_productIdHeaderId);
	imtgql::IGqlContextCreator::ContextCreationStatus status = imtgql::IGqlContextCreator::CCS_OK;
	imtgql::IGqlContextUniquePtr gqlContextPtr = m_gqlContextCreatorCompPtr->CreateGqlContext(
				token,
				productId,
				QByteArray(),
				headers,
				errorMessage,
				&status);
	if (!gqlContextPtr.IsValid()){
		SendErrorMessage(0, errorMessage, "CGqlContextControllerComp");
		return nullptr;
	}

	return gqlContextPtr.PopInterfacePtr();
}


} // namespace imtservergql
