// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CGqlControllerComp.h>


namespace imtservergql
{


// protected methods

// reimplemented (imtservergql::CGqlRequestHandlerCompBase)

imtbase::CTreeItemModel* CGqlControllerComp::CreateInternalResponse(const imtgql::CGqlRequest& /*gqlRequest*/, QString& /*errorMessage*/) const
{
	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");
	dataModelPtr->SetData("Status", "OK");

	return rootModelPtr.PopPtr();
}


} // namespace imtservergql


