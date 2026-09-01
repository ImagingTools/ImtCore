// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CGuiElementContainerRepresentationControllerComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/IIdParam.h>

// std includes
#include <algorithm>

// Qt includes
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

// ImtCore includes
#include <imtserverapp/imtserverapp.h>
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
			QJsonObject& representation,
			const iprm::IParamsSet* paramsPtr) const
{
	Q_ASSERT(IsModelSupported(dataModel));

	const imtserverapp::IGuiElementContainer* guiElementContainerPtr = dynamic_cast<const imtserverapp::IGuiElementContainer*>(&dataModel);
	if (guiElementContainerPtr == nullptr){
		return false;
	}

	representation = QJsonObject();
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

	QJsonArray itemsArray;

	const QByteArray permissionPath = GetPermissionPath(paramsPtr);

	QByteArrayList elementIds = guiElementContainerPtr->GetElementIds();
	for (const QByteArray& elementId : elementIds){
		const imtserverapp::IGuiElementModel* guiElementModelPtr = guiElementContainerPtr->GetGuiElementModel(elementId);
		if (guiElementModelPtr != nullptr){
			if (!IsElementAccessible(
						m_commandPermissionsProviderCompPtr.GetPtr(),
						m_checkPermissionCompPtr.GetPtr(),
						elementId,
						userPermissions,
						isAdmin,
						permissionPath)){
				continue;
			}

			const imtserverapp::IRepresentationController* representationControllerPtr = FindRepresentationController(elementId);
			if (representationControllerPtr != nullptr){
				QJsonObject guiElementRepresentationModel;
				if (representationControllerPtr->GetRepresentationFromDataModel(*guiElementModelPtr, guiElementRepresentationModel, paramsPtr)){
					if (guiElementRepresentationModel.contains(QStringLiteral("items")) && guiElementRepresentationModel.value(QStringLiteral("items")).isArray()){
						const QJsonArray nestedItems = guiElementRepresentationModel.value(QStringLiteral("items")).toArray();
						for (const QJsonValue& value : nestedItems){
							if (value.isObject()){
								itemsArray.append(value.toObject());
							}
						}
					}
					else{
						itemsArray.append(guiElementRepresentationModel);
					}
				}
			}
		}
	}

	if (m_slaveRepresentationControllerCompPtr.IsValid()){
		QJsonObject slaveRepresentation;
		bool ok = m_slaveRepresentationControllerCompPtr->GetRepresentationFromDataModel(*guiElementContainerPtr, slaveRepresentation, paramsPtr);
		if (!ok){
			return false;
		}
		if (slaveRepresentation.contains(QStringLiteral("items")) && slaveRepresentation.value(QStringLiteral("items")).isArray()){
			const QJsonArray nestedItems = slaveRepresentation.value(QStringLiteral("items")).toArray();
			for (const QJsonValue& value : nestedItems){
				if (value.isObject()){
					itemsArray.append(value.toObject());
				}
			}
		}
		else if (!slaveRepresentation.isEmpty()){
			itemsArray.append(slaveRepresentation);
		}
	}

	QList<QJsonObject> sortedItems;
	sortedItems.reserve(itemsArray.size());
	for (const QJsonValue& value : itemsArray){
		if (value.isObject()){
			sortedItems.append(value.toObject());
		}
	}
	std::sort(sortedItems.begin(), sortedItems.end(), [](const QJsonObject& item1, const QJsonObject& item2){
		const int alignment1 = item1.value(QStringLiteral("alignment")).toInt();
		const int alignment2 = item2.value(QStringLiteral("alignment")).toInt();
		// Keep the original behavior: top/default-aligned items go before bottom-aligned items.
		const int alignmentRank1 = alignment1 == Qt::AlignBottom ? 1 : 0;
		const int alignmentRank2 = alignment2 == Qt::AlignBottom ? 1 : 0;
		if (alignmentRank1 != alignmentRank2){
			return alignmentRank1 < alignmentRank2;
		}

		const int priority1 = item1.value(QStringLiteral("priority")).toInt();
		const int priority2 = item2.value(QStringLiteral("priority")).toInt();
		return priority1 > priority2;
	});
	itemsArray = QJsonArray();
	for (const QJsonObject& itemObj : sortedItems){
		itemsArray.append(itemObj);
	}

	representation.insert(QStringLiteral("items"), itemsArray);
	return true;
}


bool CGuiElementContainerRepresentationControllerComp::GetDataModelFromRepresentation(
			const QJsonObject& /*representation*/,
			istd::IChangeable& /*dataModel*/) const
{
	return false;
}


} // namespace imtserverapp
