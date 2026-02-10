// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtgql/IGqlContext.h>
#include <imtgql/CGqlRequest.h>


namespace imtgql
{


/**
	\todo Rename it to IGqlContextProvider and replace CGqlRequest by IGqlRequest in GetRequestContext
*/
class IGqlContextController: virtual public istd::IPolymorphic
{
public:
	virtual IGqlContext* GetRequestContext(
				const CGqlRequest& gqlRequest,
				const QByteArray& token,
				const IGqlContext::Headers& headers,
				QString& errorMessage) const = 0;
};


} // namespace imtgql


