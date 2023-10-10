#include <imtlicgql/CFeatureControllerComp.h>


// ACF includes
#include <iprm/CIdParam.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtlic/CFeatureInfo.h>


namespace imtlicgql
{


// pretected methods

// reimplemented (imtgql::CObjectCollectionControllerCompBase)

istd::IChangeable* CFeatureControllerComp::CreateObject(
			const imtgql::CGqlRequest& gqlRequest,
			QByteArray& objectId,
			QString& name,
			QString& description,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QT_TR_NOOP("Internal error.");
		SendErrorMessage(0, errorMessage, "Feature controller");

		return nullptr;
	}

	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParam("input");
	if (inputParamPtr == nullptr){
		errorMessage = QT_TR_NOOP("Unable to create object. GQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "Feature controller");

		return nullptr;
	}

	objectId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();

	QByteArray jsonItemData = inputParamPtr->GetFieldArgumentValue("Item").toByteArray();
	if (jsonItemData.isEmpty()){
		errorMessage = QT_TR_NOOP("Unable to create object from empty json.");
		SendErrorMessage(0, errorMessage, "Feature controller");

		return nullptr;
	}

	imtbase::CTreeItemModel featureModel;
	if (!featureModel.CreateFromJson(jsonItemData)){
		errorMessage = QT_TR_NOOP(QString("Unable to create tree item model from json: %1.").arg(qPrintable(jsonItemData)));
		SendErrorMessage(0, errorMessage, "Feature controller");

		return nullptr;
	}

	if (featureModel.ContainsKey("FeatureName")){
		name = featureModel.GetData("FeatureName").toString();
	}

	if (featureModel.ContainsKey("FeatureDescription")){
		description = featureModel.GetData("FeatureDescription").toString();
	}

	istd::TDelPtr<imtlic::CFeatureInfo> featureInfoPtr;
	featureInfoPtr.SetPtr(new imtlic::CFeatureInfo);

	bool ok = CreateFeatureFromRepresentationModel(featureModel, *featureInfoPtr, objectId, errorMessage);
	if (!ok){
		SendErrorMessage(0, errorMessage, "Feature controller");

		return nullptr;
	}

	return featureInfoPtr.PopPtr();
}


imtbase::CTreeItemModel* CFeatureControllerComp::GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error.").toUtf8();
		SendErrorMessage(0, errorMessage, "Feature controller");

		return nullptr;
	}

	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParam("input");
	if (inputParamPtr == nullptr){
		errorMessage = QT_TR_NOOP("Unable to get object. GQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "Feature controller");

		return nullptr;
	}

	QByteArray objectId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		const imtlic::CFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::CFeatureInfo*>(dataPtr.GetPtr());
		if (featureInfoPtr != nullptr){
			istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
			imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

			dataModelPtr->SetData("Id", objectId);

			dataModelPtr->SetData("RootFeatureId", featureInfoPtr->GetFeatureId());
			dataModelPtr->SetData("ParentFeatureId", "");

			bool ok = CreateRepresentationModelFromFeatureInfo(*featureInfoPtr, nullptr, *dataModelPtr, errorMessage);
			if (!ok){
				SendErrorMessage(0, errorMessage, "Feature controller");

				return nullptr;
			}

			if (dataModelPtr->ContainsKey("FeatureName")){
				dataModelPtr->SetData("Name", dataModelPtr->GetData("FeatureName").toString());
			}

			return rootModelPtr.PopPtr();
		}
	}

	errorMessage = QT_TR_NOOP(QString("Unable to get feature by ID: %1.").arg(qPrintable(objectId)));
	SendErrorMessage(0, errorMessage, "Feature controller");

	return nullptr;
}


imtbase::CTreeItemModel* CFeatureControllerComp::GetTreeItemModel(const imtgql::CGqlRequest& /*gqlRequest*/, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	dataModelPtr->SetIsArray(true);

	imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds();
	for (const QByteArray& collectionId : collectionIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(collectionId, dataPtr)){
			const imtlic::CFeatureInfo* featureInfoPtr  = dynamic_cast<const imtlic::CFeatureInfo*>(dataPtr.GetPtr());
			if (featureInfoPtr != nullptr){
				QByteArray featureId = featureInfoPtr->GetFeatureId();

				int index = dataModelPtr->InsertNewItem();
				dataModelPtr->SetData("Id", collectionId, index);

				istd::TDelPtr<imtbase::CTreeItemModel> featureModelPtr(new imtbase::CTreeItemModel);

				featureModelPtr->SetData("RootFeatureId", featureId);
				featureModelPtr->SetData("ParentFeatureId", "");

				bool ok = CreateRepresentationModelFromFeatureInfo(*featureInfoPtr, nullptr, *featureModelPtr, errorMessage);
				if (!ok){
					SendErrorMessage(0, errorMessage, "Feature controller");

					return nullptr;
				}

				dataModelPtr->CopyItemDataFromModel(index, featureModelPtr.PopPtr(), 0);

				dataModelPtr->SetData("RootFeatureId", featureId, index);
				dataModelPtr->SetData("ParentFeatureId", "", index);

				dataModelPtr->SetData("Id", collectionId, index);
			}
		}
	}

	return rootModelPtr.PopPtr();
}


bool CFeatureControllerComp::GetOperationFromRequest(
			const imtgql::CGqlRequest& gqlRequest,
			imtgql::CGqlObject& gqlObject,
			QString& errorMessage,
			int& operationType) const
{
	const QList<imtgql::CGqlObject> fieldList = gqlRequest.GetFields();

	int count = fieldList.count();
	for (int i = 0; i < count; i++){
		if (fieldList.at(i).GetId() == "treeItem"){
			gqlObject = fieldList.at(i);
			operationType = OT_USER_OPERATION + 1;
			return true;
		}

		if (BaseClass::GetOperationFromRequest(gqlRequest, gqlObject, errorMessage, operationType)){
			return true;
		}
	}

	return false;
}


bool CFeatureControllerComp::CreateFeatureFromRepresentationModel(
			const imtbase::CTreeItemModel& representationModel,
			imtlic::CFeatureInfo& featureInfo,
			const QByteArray& objectId,
			QString& errorMessage) const
{
	QByteArray featureId;
	if (representationModel.ContainsKey("FeatureId")){
		featureId = representationModel.GetData("FeatureId").toByteArray();
	}

	if (featureId.isEmpty()){
		errorMessage = QT_TR_NOOP(QString("Unable to create feature with an empty ID."));
		SendErrorMessage(0, errorMessage, "Feature controller");

		return false;
	}

	iprm::CIdParam idParam;
	idParam.SetId(featureId);

	iprm::CParamsSet paramsSet1;
	paramsSet1.SetEditableParameter("FeatureId", &idParam);

	iprm::CParamsSet filterParam;
	filterParam.SetEditableParameter("ObjectFilter", &paramsSet1);

	imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds(0, -1, &filterParam);
	if (!collectionIds.isEmpty()){
		QByteArray id = collectionIds[0];
		if (objectId != id){
			errorMessage = QT_TR_NOOP(QString("Feature-ID: %1 already exists. Please rename.")).arg(qPrintable(featureId));
			return false;
		}
	}

	QString featureName;
	if (representationModel.ContainsKey("FeatureName")){
		featureName = representationModel.GetData("FeatureName").toString();
	}

	if (featureName.isEmpty()){
		errorMessage = QT_TR_NOOP(QString("Unable to create feature with an empty Name."));

		return false;
	}

	iprm::CIdParam nameParam;
	nameParam.SetId(featureName.toUtf8());

	iprm::CParamsSet paramsSet2;
	paramsSet2.SetEditableParameter("FeatureName", &nameParam);

	iprm::CParamsSet filterParam2;
	filterParam2.SetEditableParameter("ObjectFilter", &paramsSet2);

	imtbase::ICollectionInfo::Ids collectionIds2 = m_objectCollectionCompPtr->GetElementIds(0, -1, &filterParam2);
	if (!collectionIds2.isEmpty()){
		QByteArray id = collectionIds2[0];
		if (objectId != id){
			errorMessage = QT_TR_NOOP(QString("Feature Name: %1 already exists. Please rename.").arg(featureName));
			return false;
		}
	}

	bool isOptional = false;
	if (representationModel.ContainsKey("Optional")){
		isOptional = representationModel.GetData("Optional").toBool();
	}

	featureInfo.SetFeatureId(featureId);
	featureInfo.SetFeatureName(featureName);
	featureInfo.SetOptional(isOptional);

	QString featureDescription;
	if (representationModel.ContainsKey("FeatureDescription")){
		featureDescription = representationModel.GetData("FeatureDescription").toString();
	}

	featureInfo.SetFeatureDescription(featureDescription);

	if (representationModel.ContainsKey("ChildModel")){
		imtbase::CTreeItemModel* subFeaturesModelPtr = representationModel.GetTreeItemModel("ChildModel");
		if (subFeaturesModelPtr != nullptr){
			for (int i = 0; i < subFeaturesModelPtr->GetItemsCount(); i++){
				imtbase::CTreeItemModel* featureRepresentationModelPtr = subFeaturesModelPtr->GetModelFromItem(i);
				Q_ASSERT(featureRepresentationModelPtr != nullptr);

				imtlic::CFeatureInfo* subFeatureInfoPtr = new imtlic::CFeatureInfo();
				bool ok = CreateFeatureFromRepresentationModel(*featureRepresentationModelPtr, *subFeatureInfoPtr, objectId, errorMessage);
				if (!ok){
					return false;
				}

				featureInfo.InsertSubFeature(subFeatureInfoPtr);
			}
		}
	}

	QByteArray dependencies;
	if (representationModel.ContainsKey("Dependencies")){
		dependencies = representationModel.GetData("Dependencies").toByteArray();
	}

	QByteArrayList featureIds = dependencies.split(';');
	if (!featureIds.isEmpty()){
		featureInfo.SetDependencies(featureIds);
	}

	return true;
}


bool CFeatureControllerComp::CreateRepresentationModelFromFeatureInfo(
			const imtlic::CFeatureInfo& featureInfo,
			const imtbase::CTreeItemModel* parentModelPtr,
			imtbase::CTreeItemModel& representationModel,
			QString& errorMessage) const
{
	QByteArray featureId = featureInfo.GetFeatureId();

	representationModel.SetData("FeatureId", featureId);
	representationModel.SetData("FeatureName", featureInfo.GetFeatureName());
	representationModel.SetData("Optional", featureInfo.IsOptional());
	representationModel.SetData("FeatureDescription", featureInfo.GetFeatureDescription());
	representationModel.SetData("Dependencies", featureInfo.GetDependencies().join(';'));
	representationModel.SetData("ChildModel", 0);

	if (parentModelPtr != nullptr){
		if (parentModelPtr->ContainsKey("FeatureId")){
			representationModel.SetData("ParentFeatureId", parentModelPtr->GetData("FeatureId"));
		}

		if (parentModelPtr->ContainsKey("RootFeatureId")){
			representationModel.SetData("RootFeatureId", parentModelPtr->GetData("RootFeatureId"));
		}
	}

	const imtlic::FeatureInfoList& subFeatures = featureInfo.GetSubFeatures();
	if (!subFeatures.IsEmpty()){
		imtbase::CTreeItemModel* childModelPtr = representationModel.AddTreeModel("ChildModel");
		Q_ASSERT(childModelPtr != nullptr);

		for (int i = 0; i < subFeatures.GetCount(); i++){
			const imtlic::IFeatureInfo* featureInfoPtr = subFeatures.GetAt(i);
			if (featureInfoPtr == nullptr){
				errorMessage = QString("Unable to create representation model for invalid subfeature. Parent feature id: %1.").arg(qPrintable(featureId));
				return false;
			}

			const imtlic::CFeatureInfo* subFeatureInfoPtr = dynamic_cast<const imtlic::CFeatureInfo*>(featureInfoPtr);
			Q_ASSERT(subFeatureInfoPtr != nullptr);

			imtbase::CTreeItemModel subFeatureRepresentationModel;
			bool ok = CreateRepresentationModelFromFeatureInfo(*subFeatureInfoPtr, &representationModel, subFeatureRepresentationModel, errorMessage);
			if (!ok){
				return false;
			}

			childModelPtr->InsertNewItem();
			childModelPtr->CopyItemDataFromModel(i, &subFeatureRepresentationModel, 0);
		}
	}

	return true;
}


} // namespace imtlicgql


