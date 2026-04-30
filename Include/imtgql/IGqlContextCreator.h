// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtgql/IGqlContext.h>
#include <imtgql/CGqlRequest.h>

// Qt includes
#include <QtCore/QString>


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

	struct ContextCreationError
	{
		ContextCreationStatus status = CCS_OK;
		QString message;
	};

	virtual IGqlContextUniquePtr CreateGqlContext(
				const IGqlContext::Headers& headers,
				ContextCreationError& error) const = 0;
};


} // namespace imtgql
