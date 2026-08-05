// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlicgql/CFeatureControllerComp.h>

// Qt includes
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>

// ACF includes
#include <iprm/CIdParam.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ImtCollection.h>
#include <imtlic/CFeatureInfo.h>


namespace imtlicgql
{


// pretected methods

// reimplemented (imtservergql::CObjectCollectionControllerCompBase)

istd::IChangeableUniquePtr CFeatureControllerComp::CreateObjectFromRequest(
			const imtgql::CGqlRequest& gqlRequest,
			QByteArray& objectId,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QT_TR_NOOP("Internal error.");
		SendErrorMessage(0, errorMessage, "Feature controller");

		return nullptr;
	}

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QT_TR_NOOP("Unable to create object. GQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "Feature controller");

		return nullptr;
	}

	objectId = inputParamPtr->GetParamArgumentValue("Id").toByteArray();
	if (objectId.isEmpty()){
		objectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}

	QByteArray jsonItemData = inputParamPtr->GetParamArgumentValue("Item").toByteArray();
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

	QString name;
	QString description;

	if (featureModel.ContainsKey("FeatureName")){
		name = featureModel.GetData("FeatureName").toString();
	}

	if (featureModel.ContainsKey("FeatureDescription")){
		description = featureModel.GetData("FeatureDescription").toString();
	}

	istd::TDelPtr<imtlic::CIdentifiableFeatureInfo> featureInfoPtr;
	featureInfoPtr.SetPtr(new imtlic::CIdentifiableFeatureInfo);

	featureInfoPtr->SetObjectUuid(objectId);

	bool ok = CreateFeatureFromRepresentationModel(featureModel, *featureInfoPtr, objectId, errorMessage);
	if (!ok){
		SendErrorMessage(0, errorMessage, "Feature controller");

		return nullptr;
	}

	return featureInfoPtr.PopPtr();
}


QJsonObject CFeatureControllerComp::GetObjectFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Internal error.").toUtf8();
		SendErrorMessage(0, errorMessage, "Feature controller");

		return QJsonObject();
	}

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QT_TR_NOOP("Unable to get object. GQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "Feature controller");

		return QJsonObject();
	}

	QByteArray objectId = inputParamPtr->GetParamArgumentValue("Id").toByteArray();

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		const imtlic::CIdentifiableFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::CIdentifiableFeatureInfo*>(dataPtr.GetPtr());
		if (featureInfoPtr != nullptr){
			imtbase::CTreeItemModel tempModel;
			tempModel.SetData("Id", objectId);
			tempModel.SetData("RootFeatureId", featureInfoPtr->GetFeatureId());
			tempModel.SetData("ParentFeatureId", "");

			bool ok = CreateRepresentationModelFromFeatureInfo(*featureInfoPtr, nullptr, tempModel, errorMessage);
			if (!ok){
				SendErrorMessage(0, errorMessage, "Feature controller");

				return QJsonObject();
			}

			if (tempModel.ContainsKey("FeatureName")){
				tempModel.SetData("Name", tempModel.GetData("FeatureName").toString());
			}

			QJsonObject rootObj;
			QString jsonStr = tempModel.ToJson();
			QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
			if (doc.isObject()){
				rootObj.insert(QStringLiteral("data"), doc.object());
			}
			else if (doc.isArray() && !doc.array().isEmpty()){
				rootObj.insert(QStringLiteral("data"), doc.array().first().toObject());
			}

			return rootObj;
		}
	}

	errorMessage = QT_TR_NOOP(QString("Unable to get feature by ID: %1.").arg(qPrintable(objectId)));
	SendErrorMessage(0, errorMessage, "Feature controller");

	return QJsonObject();
}


QJsonObject CFeatureControllerComp::GetTreeItemModel(const imtgql::CGqlRequest& /*gqlRequest*/, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return QJsonObject();
	}

	QJsonObject rootObj;
	QJsonArray dataArray;

	imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds();
	for (const QByteArray& collectionId : collectionIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(collectionId, dataPtr)){
			const imtlic::CFeatureInfo* featureInfoPtr  = dynamic_cast<const imtlic::CFeatureInfo*>(dataPtr.GetPtr());
			if (featureInfoPtr != nullptr){
				QByteArray featureId = featureInfoPtr->GetFeatureId();

				imtbase::CTreeItemModel featureModel;
				featureModel.SetData("RootFeatureId", featureId);
				featureModel.SetData("ParentFeatureId", "");

				bool ok = CreateRepresentationModelFromFeatureInfo(*featureInfoPtr, nullptr, featureModel, errorMessage);
				if (!ok){
					SendErrorMessage(0, errorMessage, "Feature controller");

					return QJsonObject();
				}

				QJsonObject itemObj;
				QString jsonStr = featureModel.ToJson();
				QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
				if (doc.isObject()){
					itemObj = doc.object();
				}
				else if (doc.isArray() && !doc.array().isEmpty()){
					itemObj = doc.array().first().toObject();
				}

				itemObj.insert(QStringLiteral("RootFeatureId"), QJsonValue::fromVariant(featureId));
				itemObj.insert(QStringLiteral("ParentFeatureId"), QJsonValue::fromVariant(QString("")));
				itemObj.insert(QStringLiteral("Id"), QJsonValue::fromVariant(collectionId));

				dataArray.append(itemObj);
			}
		}
	}

	rootObj.insert(QStringLiteral("data"), dataArray);
	return rootObj;
}


bool CFeatureControllerComp::GetOperationFromRequest(
			const imtgql::CGqlRequest& gqlRequest,
			imtgql::CGqlParamObject& gqlObject,
			QString& errorMessage,
			int& operationType) const
{
	const imtgql::CGqlFieldObject& fieldList = gqlRequest.GetFields();

	if (fieldList.GetFieldIds().contains("treeItem")){
		return true;
	}

	if (BaseClass::GetOperationFromRequest(gqlRequest, gqlObject, errorMessage, operationType)){
		return true;
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
			errorMessage = QT_TR_NOOP(QString("Feature-ID: '%1' already exists. Please rename.")).arg(qPrintable(featureId));
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

	QByteArrayList featureIds;
	if (!dependencies.isEmpty()){
		featureIds = dependencies.split(';');
	}

	featureInfo.SetDependencies(featureIds);

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
	representationModel.AddTreeModel("ChildModel");

	if (parentModelPtr != nullptr){
		if (parentModelPtr->ContainsKey("FeatureId")){
			representationModel.SetData("ParentFeatureId", parentModelPtr->GetData("FeatureId"));
		}

		if (parentModelPtr->ContainsKey("RootFeatureId")){
			representationModel.SetData("RootFeatureId", parentModelPtr->GetData("RootFeatureId"));
		}
	}

	const imtlic::IFeatureInfo::FeatureInfoList& subFeatures = featureInfo.GetSubFeatures();
	if (!subFeatures.isEmpty()){
		imtbase::CTreeItemModel* childModelPtr = representationModel.AddTreeModel("ChildModel");
		Q_ASSERT(childModelPtr != nullptr);

		for (int i = 0; i < subFeatures.count(); i++){
			imtlic::IFeatureInfoSharedPtr featureInfoPtr = subFeatures.at(i);
			if (!featureInfoPtr.IsValid()){
				errorMessage = QString("Unable to create representation model for invalid subfeature. Parent feature id: %1.").arg(qPrintable(featureId));
				return false;
			}

			const imtlic::CFeatureInfo* subFeatureInfoPtr = dynamic_cast<const imtlic::CFeatureInfo*>(featureInfoPtr.GetPtr());
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


