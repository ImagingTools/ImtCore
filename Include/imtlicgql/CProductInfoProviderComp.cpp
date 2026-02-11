// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlicgql/CProductInfoProviderComp.h>


namespace imtlicgql
{


// protected methods

// reimplemented (imtservergql::CGqlRequestHandlerCompBase)

bool CProductInfoProviderComp::IsRequestSupported(const imtgql::CGqlRequest& /*gqlRequest*/) const
{
	return true;
}


imtbase::CTreeItemModel* CProductInfoProviderComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	for (int i = 0; i < m_productIdsAttrPtr.GetCount(); i++){
		int index = rootModelPtr->InsertNewItem();

		rootModelPtr->SetData("id", m_productIdsAttrPtr[i], index);
		rootModelPtr->SetData("name", m_productNamesAttrPtr[i], index);

		imtgql::IGqlRequestHandler* representationDataProvider = m_permissionsProviderCompPtr[i];
		if (representationDataProvider != nullptr){
			imtbase::CTreeItemModel* permissionsModelPtr = representationDataProvider->CreateResponse(gqlRequest, errorMessage);
			if (permissionsModelPtr != nullptr){
				rootModelPtr->SetExternTreeModel("permissions", permissionsModelPtr, i);
			}
		}
	}

	return rootModelPtr.PopPtr();
}


} // namespace imtlicgql


