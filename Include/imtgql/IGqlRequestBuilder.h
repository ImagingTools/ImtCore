// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtgql/IGqlRequest.h>


namespace imtgql
{


class IGqlRequestBuilder: virtual public istd::IPolymorphic
{
public:
	virtual const IGqlRequest* CreateRequest(const QByteArray& commandId) const = 0;
};


} // namespace imtgql


