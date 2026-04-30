// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtgql/IGqlContext.h>
#include <imtgql/CGqlRequest.h>


namespace imtgql
{


class IGqlContextCreator: virtual public istd::IPolymorphic
{
public:
	enum ContextCreationStatus
	{
		CCS_OK,
		CCS_UNAUTHORIZED,
		CCS_FORBIDDEN,
		CCS_INTERNAL_ERROR
	};

	virtual IGqlContextUniquePtr CreateGqlContext(
				const QByteArray& token,
				const QByteArray& productId,
				const QByteArray& userId,
				const IGqlContext::Headers& headers,
				QString& errorMessage,
				ContextCreationStatus* statusPtr = nullptr) const = 0;
};


} // namespace imtgql

