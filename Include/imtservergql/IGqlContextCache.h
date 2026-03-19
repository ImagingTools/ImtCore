// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtgql/IGqlContext.h>


namespace imtservergql
{


class IGqlContextCache: virtual public istd::IPolymorphic
{
public:
	virtual bool TryCreateRequestContext(
				const QByteArray& token,
				const QByteArray& productId,
				const imtgql::IGqlContext::Headers& headers,
				imtgql::IGqlContextSharedPtr& gqlContextPtr) const = 0;

	virtual void Insert(
				const QByteArray& token,
				const imtgql::IGqlContext* gqlContextPtr) const = 0;
};


} // namespace imtservergql

