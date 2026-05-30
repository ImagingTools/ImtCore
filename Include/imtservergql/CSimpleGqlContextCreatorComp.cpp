// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CSimpleGqlContextCreatorComp.h>


// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtgql/CGqlRequestContextManager.h>


namespace imtservergql
{


// public methods

// reimplemented (imtgql::IGqlContextCreator)

imtgql::IGqlContextUniquePtr CSimpleGqlContextCreatorComp::CreateGqlContext(
			const imtgql::IGqlContext::Headers& headers,
			imtgql::IGqlContextCreator::ContextCreationError& error) const
{
	error.status = imtgql::IGqlContextCreator::CCS_OK;
	error.message.clear();

	if (!m_gqlContextFactCompPtr.IsValid()){
		error.status = imtgql::IGqlContextCreator::CCS_INTERNAL_ERROR;
		error.message = QStringLiteral("GraphQL context factory is not configured.");
		return nullptr;
	}

	imtgql::IGqlContextUniquePtr gqlContextPtr = m_gqlContextFactCompPtr.CreateInstance();
	if (!gqlContextPtr.IsValid()){
		error.status = imtgql::IGqlContextCreator::CCS_INTERNAL_ERROR;
		error.message = QStringLiteral("Unable to create GraphQL context instance.");
		return nullptr;
	}

	gqlContextPtr->SetHeaders(headers);
	gqlContextPtr->SetToken(headers.value(imtbase::s_authenticationTokenHeaderId));
	gqlContextPtr->SetProductId(headers.value(imtbase::s_productIdHeaderId));

	imtgql::CGqlRequestContextManager::SetContext(dynamic_cast<imtgql::IGqlContext*>(gqlContextPtr.GetPtr()));

	return gqlContextPtr;
}


} // namespace imtservergql
