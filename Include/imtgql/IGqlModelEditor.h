// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtgql/IGqlRepresentationDataController.h>


namespace imtgql
{


class IGqlModelEditor: virtual public imtservergql::CGqlRepresentationDataControllerComp
{
public:
	virtual bool UpdateModelFromRepresentation(const CGqlRequest& request, imtbase::CTreeItemModel* modelPtr) = 0;
};


} // namespace imtgql


