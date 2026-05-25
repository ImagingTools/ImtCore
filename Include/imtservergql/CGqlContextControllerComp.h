// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

#include <imtgql/IGqlContextController.h>
#include <imtgql/IGqlContextCreator.h>


namespace imtservergql
{


class CGqlContextControllerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtgql::IGqlContextController
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGqlContextControllerComp);
		I_REGISTER_INTERFACE(imtgql::IGqlContextController);
		I_ASSIGN(m_gqlContextCreatorCompPtr, "GqlContextCreator", "GraphQL-related context creator", false, "AuthenticationManager");
	I_END_COMPONENT;

	// reimplemented (imtgql::IGqlContextController)
	virtual imtgql::IGqlContext* GetRequestContext(
				const imtgql::CGqlRequest& gqlRequest,
				const QByteArray& token,
				const imtgql::IGqlContext::Headers& headers,
				QString& errorMessage) const override;

private:
	I_REF(imtgql::IGqlContextCreator, m_gqlContextCreatorCompPtr);
};


} // namespace imtservergql

