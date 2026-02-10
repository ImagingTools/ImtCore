// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtgql/IGqlContextCreator.h>


namespace imtservergql
{


class CGqlContextCreatorComp:
			public ilog::CLoggerComponentBase,
			virtual public imtgql::IGqlContextCreator
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGqlContextCreatorComp);
		I_REGISTER_INTERFACE(imtgql::IGqlContextCreator);
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "User collection", false, "UserCollection");
		I_ASSIGN(m_userSettingsCollectionCompPtr, "UserSettingsCollection", "User settings collection", false, "UserSettingsCollection");
		I_ASSIGN(m_gqlContextFactCompPtr, "GqlContextFactory", "GraphQL context factory", true, "GqlContextFactory");
	I_END_COMPONENT;

	// reimplemented (imtgql::IGqlContextCreator)
	virtual imtgql::IGqlContextUniquePtr CreateGqlContext(
				const QByteArray& token,
				const QByteArray& productId,
				const QByteArray& userId,
				const imtgql::IGqlContext::Headers& headers,
				QString& errorMessage) const override;

private:
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_userSettingsCollectionCompPtr);
	I_FACT(imtgql::IGqlContext, m_gqlContextFactCompPtr);
};


} // namespace imtservergql


