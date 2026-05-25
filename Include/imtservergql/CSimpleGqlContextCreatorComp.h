// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtgql/IGqlContextCreator.h>


namespace imtservergql
{


/**
	GraphQL context creator for systems without authorization.

	Creates an IGqlContext instance and populates it from the request headers
	(token, product id, full headers map) without resolving any user information.
	Use it when the surrounding system does not perform authentication.
*/
class CSimpleGqlContextCreatorComp:
			public ilog::CLoggerComponentBase,
			virtual public imtgql::IGqlContextCreator
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSimpleGqlContextCreatorComp);
		I_REGISTER_INTERFACE(imtgql::IGqlContextCreator);
		I_ASSIGN(m_gqlContextFactCompPtr, "GqlContextFactory", "GraphQL context factory", true, "GqlContextFactory");
	I_END_COMPONENT;

	// reimplemented (imtgql::IGqlContextCreator)
	virtual imtgql::IGqlContextUniquePtr CreateGqlContext(
				const imtgql::IGqlContext::Headers& headers,
				imtgql::IGqlContextCreator::ContextCreationError& error) const override;

private:
	I_FACT(imtgql::IGqlContext, m_gqlContextFactCompPtr);
};


} // namespace imtservergql
