// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace imtgql
{


class IGqlRequestExtractor: virtual public istd::IPolymorphic
{
public:

	/**
		Create object from the GraphQL
	*/
	virtual istd::IChangeableUniquePtr ExtractObject(const CGqlRequest& gqlRequest, QByteArray& newObjectId, QString& errorMessage) const = 0;
};


} // namespace imtgql


