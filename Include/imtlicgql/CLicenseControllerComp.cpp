#include <imtlicgql/CLicenseControllerComp.h>


// ACF includes
#include <iprm/CIdParam.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtlic/CLicenseDefinition.h>


namespace imtlicgql
{


istd::IChangeable* CLicenseControllerComp::CreateObject(
			const imtgql::CGqlRequest& gqlRequest,
			QByteArray& objectId,
			QString& name,
			QString& description,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QT_TR_NOOP("Internal error.");
		SendErrorMessage(0, errorMessage, "License controller");

		return nullptr;
	}

	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParam("input");
	if (inputParamPtr == nullptr){
		errorMessage = QT_TR_NOOP("Unable to create object. GQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "License controller");

		return nullptr;
	}

	objectId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();
	if (objectId.isEmpty()){
		objectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}

	QByteArray jsonItemData = inputParamPtr->GetFieldArgumentValue("Item").toByteArray();
	if (jsonItemData.isEmpty()){
		errorMessage = QT_TR_NOOP("Unable to create object from empty json.");
		SendErrorMessage(0, errorMessage, "License controller");

		return nullptr;
	}

	imtbase::CTreeItemModel licenseModel;
	if (!licenseModel.CreateFromJson(jsonItemData)){
		errorMessage = QT_TR_NOOP(QString("Unable to create tree item model from json: %1.").arg(qPrintable(jsonItemData)));
		SendErrorMessage(0, errorMessage, "License controller");

		return nullptr;
	}

	istd::TDelPtr<imtlic::CIdentifiableLicenseDefinition> licenseInfoPtr;
	licenseInfoPtr.SetPtr(new imtlic::CIdentifiableLicenseDefinition);

	licenseInfoPtr->SetObjectUuid(objectId);

	QByteArray licenseId;
	if (licenseModel.ContainsKey("LicenseId")){
		licenseId = licenseModel.GetData("LicenseId").toByteArray().trimmed();
	}

	if (licenseId.isEmpty()){
		errorMessage = QT_TR_NOOP("Unable to create license with an empty License-ID.");
		SendErrorMessage(0, errorMessage, "License controller");

		return nullptr;
	}

	iprm::CIdParam idParam;
	idParam.SetId(licenseId);

	iprm::CParamsSet paramsSet1;
	paramsSet1.SetEditableParameter("LicenseId", &idParam);

	iprm::CParamsSet filterParam;
	filterParam.SetEditableParameter("ObjectFilter", &paramsSet1);

	imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds(0, -1, &filterParam);
	if (!collectionIds.isEmpty()){
		QByteArray id = collectionIds[0];
		if (objectId != id){
			errorMessage = QT_TR_NOOP(QString("License-ID: %1 already exists.")).arg(qPrintable(licenseId));

			return nullptr;
		}
	}

	licenseInfoPtr->SetLicenseId(licenseId);

	if (licenseModel.ContainsKey("LicenseName")){
		name = licenseModel.GetData("LicenseName").toString().trimmed();
	}

	iprm::CIdParam nameParam;
	nameParam.SetId(name.toUtf8());

	iprm::CParamsSet paramsSet2;
	paramsSet2.SetEditableParameter("LicenseName", &nameParam);

	iprm::CParamsSet filterParam2;
	filterParam2.SetEditableParameter("ObjectFilter", &paramsSet2);

	imtbase::ICollectionInfo::Ids collectionIds2 = m_objectCollectionCompPtr->GetElementIds(0, -1, &filterParam2);
	if (!collectionIds2.isEmpty()){
		QByteArray id = collectionIds2[0];
		if (objectId != id){
			errorMessage = QT_TR_NOOP(QString("License Name: %1 already exists. Please rename.")).arg(qPrintable(name));

			return nullptr;
		}
	}

	licenseInfoPtr->SetLicenseName(name);

	if (licenseModel.ContainsKey("LicenseDescription")){
		description = licenseModel.GetData("LicenseDescription").toString();
	}

	licenseInfoPtr->SetLicenseDescription(description);

	QByteArray productId;
	if (licenseModel.ContainsKey("ProductId")){
		productId = licenseModel.GetData("ProductId").toByteArray().trimmed();
	}

	licenseInfoPtr->SetProductId(productId);

	QByteArray features;
	if (licenseModel.ContainsKey("Features")){
		features = licenseModel.GetData("Features").toByteArray();
	}

	QByteArrayList featureIds;
	if (!features.isEmpty()){
		featureIds = features.split(';');
	}

	imtlic::ILicenseDefinition::FeatureInfos featureInfos;
	for (const QByteArray& featureUuid : featureIds){
		imtlic::ILicenseDefinition::FeatureInfo featureInfo;

		featureInfo.id = featureUuid;

		featureInfos << featureInfo;
	}

	licenseInfoPtr->SetFeatureInfos(featureInfos);

	QByteArray dependencies;
	if (licenseModel.ContainsKey("ParentLicenses")){
		dependencies = licenseModel.GetData("ParentLicenses").toByteArray();
	}

	QByteArrayList dependencyIds;

	if (!dependencies.isEmpty()){
		dependencyIds = dependencies.split(';');
	}

	licenseInfoPtr->SetDependencies(dependencyIds);

	return licenseInfoPtr.PopPtr();
}


imtbase::CTreeItemModel* CLicenseControllerComp::GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
		SendErrorMessage(0, errorMessage, "License controller");

		return nullptr;
	}

	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParam("input");
	if (inputParamPtr == nullptr){
		errorMessage = QT_TR_NOOP("Unable to get object. GQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "License controller");

		return nullptr;
	}

	QByteArray objectId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		imtlic::ILicenseDefinition* licenseInfoPtr = dynamic_cast<imtlic::ILicenseDefinition*>(dataPtr.GetPtr());
		if (licenseInfoPtr != nullptr){
			istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
			imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

			dataModelPtr->SetData("Id", objectId);
			dataModelPtr->SetData("Name", licenseInfoPtr->GetLicenseName());

			dataModelPtr->SetData("LicenseId", licenseInfoPtr->GetLicenseId());
			dataModelPtr->SetData("LicenseName", licenseInfoPtr->GetLicenseName());
			dataModelPtr->SetData("LicenseDescription", licenseInfoPtr->GetLicenseDescription());
			dataModelPtr->SetData("ProductId", licenseInfoPtr->GetProductId());

			QByteArrayList featureUuids;
			for (const imtlic::ILicenseDefinition::FeatureInfo& featureInfo : licenseInfoPtr->GetFeatureInfos()){
				featureUuids << featureInfo.id;
			}

			dataModelPtr->SetData("Features", featureUuids.join(';'));
			dataModelPtr->SetData("ParentLicenses", licenseInfoPtr->GetDependencies().join(';'));

			return rootModelPtr.PopPtr();
		}
	}

	errorMessage = QT_TR_NOOP(QString("Unable to get license by ID: %1.").arg(qPrintable(objectId)));
	SendErrorMessage(0, errorMessage, "License controller");

	return nullptr;
}


} // namespace imtlicgql


