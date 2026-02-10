// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtgql/CGqlRequest.h>


namespace imtgql
{


class IGqlRepresentationController: virtual public istd::IPolymorphic
{
public:
	virtual imtbase::CTreeItemModel* CreateRepresentationFromRequest(
				const CGqlRequest& gqlRequest,
				QString& errorMessage) const = 0;
	virtual bool UpdateModelFromRepresentation(
				const CGqlRequest& request,
				imtbase::CTreeItemModel* representationPtr) const = 0;
};


} // namespace imtgql


