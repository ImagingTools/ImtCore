// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CGuiElementContainerRepresentationControllerComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/IIdParam.h>

// ImtCore includes
#include <imtserverapp/IGuiElementContainer.h>
#include <imtserverapp/IGuiElementModel.h>


namespace imtserverapp
{


// protected methods

const imtserverapp::IRepresentationController* CGuiElementContainerRepresentationControllerComp::FindRepresentationController(const QByteArray& modelId) const
{
	if (!m_representationControllersCompPtr.IsValid()){
		return nullptr;
	}

	int count = m_representationControllersCompPtr.GetCount();

	for (int i = 0; i < count; i++){
		imtserverapp::IRepresentationController* representationControllerPtr = m_representationControllersCompPtr[i];
		if (representationControllerPtr != nullptr){
			QByteArray id = representationControllerPtr->GetModelId();
			if (modelId == id){
				return representationControllerPtr;
			}
		}
	}

	return nullptr;
}


// reimplemented (imtserverapp::IRepresentationController)

QByteArray CGuiElementContainerRepresentationControllerComp::GetModelId() const
{
	return QByteArray();
}


bool CGuiElementContainerRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const imtserverapp::IGuiElementContainer* guiElementPtr = dynamic_cast<const imtserverapp::IGuiElementContainer*>(&dataModel);

	return guiElementPtr != nullptr;
}


bool CGuiElementContainerRepresentationControllerComp::GetRepresentationFromDataModel(
			const istd::IChangeable& dataModel,
			imtbase::CTreeItemModel& representation,
			const iprm::IParamsSet* paramsPtr) const
{
	Q_ASSERT(IsModelSupported(dataModel));

	const imtserverapp::IGuiElementContainer* guiElementContainerPtr = dynamic_cast<const imtserverapp::IGuiElementContainer*>(&dataModel);
	if (guiElementContainerPtr == nullptr){
		return false;
	}

	QByteArray productId;
	iprm::TParamsPtr<iprm::IIdParam> productIdParamPtr(paramsPtr, "ProductId");
	if (productIdParamPtr.IsValid()){
		productId = productIdParamPtr->GetId();
	}

	iprm::TParamsPtr<imtauth::IUserInfo> userInfoParamPtr(paramsPtr, "UserInfo");

	imtauth::IUserInfo::FeatureIds userPermissions;
	bool isAdmin = false;
	if (userInfoParamPtr.IsValid()){
		if (!productId.isEmpty()){
			userPermissions = userInfoParamPtr->GetPermissions(productId);
		}

		isAdmin = userInfoParamPtr->IsAdmin();
	}

	QByteArrayList elementIds = guiElementContainerPtr->GetElementIds();
	for (const QByteArray& elementId : elementIds){
		const imtserverapp::IGuiElementModel* guiElementModelPtr = guiElementContainerPtr->GetGuiElementModel(elementId);
		if (guiElementModelPtr != nullptr){
			if (!isAdmin){
				if (m_commandPermissionsProviderCompPtr.IsValid() && !elementId.isEmpty()){
					QByteArrayList elementPermissions = m_commandPermissionsProviderCompPtr->GetCommandPermissions(elementId);

					if (m_checkPermissionCompPtr.IsValid()){
						bool result = m_checkPermissionCompPtr->CheckPermission(userPermissions, elementPermissions);
						if (!result){
							continue;
						}
					}
				}
			}

			const imtserverapp::IRepresentationController* representationControllerPtr = FindRepresentationController(elementId);
			if (representationControllerPtr != nullptr){
				imtbase::CTreeItemModel guiElementRepresentationModel;
				if (representationControllerPtr->GetRepresentationFromDataModel(*guiElementModelPtr, guiElementRepresentationModel, paramsPtr)){
					int index = representation.InsertNewItem();
					representation.CopyItemDataFromModel(index, &guiElementRepresentationModel);
				}
			}
		}
	}

	if (m_slaveRepresentationControllerCompPtr.IsValid()){
		bool ok = m_slaveRepresentationControllerCompPtr->GetRepresentationFromDataModel(*guiElementContainerPtr, representation, paramsPtr);
		if (!ok){
			return false;
		}
	}

	int n = representation.GetItemsCount();
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n - i - 1; j++) {
			int alignment1 = representation.GetData("alignment", j).toInt();
			int alignment2 = representation.GetData("alignment", j + 1).toInt();
			int priority1   = representation.GetData("priority", j).toInt();
			int priority2   = representation.GetData("priority", j + 1).toInt();

			bool needSwap = false;

			if (alignment1 != alignment2){
				needSwap = (alignment1 == Qt::AlignBottom && alignment2 == Qt::AlignTop);
			}
			else {
				needSwap = (priority1 < priority2);
			}

			if (needSwap) {
				representation.swapItems(j, j + 1);
			}
		}
	}

	return true;
}


bool CGuiElementContainerRepresentationControllerComp::GetDataModelFromRepresentation(
			const imtbase::CTreeItemModel& /*representation*/,
			istd::IChangeable& /*dataModel*/) const
{
	return false;
}


} // namespace imtserverapp


