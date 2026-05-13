// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonObject>

// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace imtgql
{


class IGqlRepresentationController: virtual public istd::IPolymorphic
{
public:
	virtual QJsonObject CreateRepresentationFromRequest(
				const CGqlRequest& gqlRequest,
				QString& errorMessage) const = 0;
	virtual bool UpdateModelFromRepresentation(
				const CGqlRequest& request,
				const QJsonObject& representation) const = 0;
};


} // namespace imtgql


