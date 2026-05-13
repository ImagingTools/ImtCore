// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonObject>

// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace imtgql
{


class IGqlRequestHandler: virtual public istd::IPolymorphic
{
public:
	/**
		Get IDs of the supported commands.
	*/
	virtual bool IsRequestSupported(const CGqlRequest& gqlRequest) const = 0;

	/**
		Create response for a GraphQL-based request.
	*/
	virtual QJsonObject CreateResponse(const CGqlRequest& gqlRequest, QString& errorMessage) const = 0;
};


} // namespace imtgql


