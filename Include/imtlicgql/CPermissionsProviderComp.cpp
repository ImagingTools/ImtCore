// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlicgql/CPermissionsProviderComp.h>


// Qt includes
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>

// ACF includes
#include <iqt/iqt.h>


namespace imtlicgql
{


// protected methods

bool CPermissionsProviderComp::CreateRepresentationModelFromFeatureInfo(
			const imtlic::CFeatureInfo& featureInfo,
			imtbase::CTreeItemModel& representationModel,
			const QByteArray& languageId,
			QString& errorMessage) const
{
	if (!featureInfo.IsPermission()){
		return false;
	}

	QByteArray featureId = featureInfo.GetFeatureId();

	representationModel.SetData("FeatureId", featureId);

	QString featureName = featureInfo.GetFeatureName();
	if (m_translationManagerCompPtr.IsValid()){
		QString featureNameTr = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), featureName.toUtf8(), languageId, QByteArrayLiteral("Feature"));
		featureName = featureNameTr;
	}

	representationModel.SetData("FeatureName", featureName);
	representationModel.SetData("Optional", featureInfo.IsOptional());

	QString featureDescription = featureInfo.GetFeatureDescription();
	if (m_translationManagerCompPtr.IsValid()){
		QString featureDescriptionTr = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), featureDescription.toUtf8(), languageId, QByteArrayLiteral("Feature"));
		featureDescription = featureDescriptionTr;
	}

	representationModel.SetData("FeatureDescription", featureDescription);
	representationModel.SetData("Dependencies", featureInfo.GetDependencies().join(';'));
	representationModel.SetData("ChildModel", 0);

	const imtlic::IFeatureInfo::FeatureInfoList& subFeatures = featureInfo.GetSubFeatures();
	if (!subFeatures.isEmpty()){
		imtbase::CTreeItemModel* childModelPtr = representationModel.AddTreeModel("ChildModel");
		Q_ASSERT(childModelPtr != nullptr);

		for (int i = 0; i < subFeatures.count(); i++){
			const imtlic::IFeatureInfo::FeatureInfoPtr& featureInfoPtr = subFeatures.at(i);
			if (!featureInfoPtr.IsValid()){
				errorMessage = QString("Unable to create representation model for invalid subfeature. Parent feature id: %1.").arg(qPrintable(featureId));
				return false;
			}

			const imtlic::CFeatureInfo* subFeatureInfoPtr = dynamic_cast<const imtlic::CFeatureInfo*>(featureInfoPtr.GetPtr());
			Q_ASSERT(subFeatureInfoPtr != nullptr);

			imtbase::CTreeItemModel subFeatureRepresentationModel;
			bool ok = CreateRepresentationModelFromFeatureInfo(*subFeatureInfoPtr, subFeatureRepresentationModel, languageId, errorMessage);
			if (ok){
				childModelPtr->InsertNewItem();
				childModelPtr->CopyItemDataFromModel(i, &subFeatureRepresentationModel, 0);
			}
		}
	}

	return true;
}


// reimplemented (imtservergql::CGqlRequestHandlerCompBase)

QJsonObject CPermissionsProviderComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& /*errorMessage*/) const
{
	if (!m_productInfoCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'FeatureContainer' was not set", "CPermissionsProviderComp");
		return QJsonObject();
	}

	QByteArray languageId;
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		languageId = gqlContextPtr->GetLanguageId();
	}

	QJsonObject resultObj;
	QJsonArray dataArray;

	imtbase::IObjectCollection* featureCollectionPtr = m_productInfoCompPtr->GetFeatures();
	if (featureCollectionPtr != nullptr){
		imtbase::ICollectionInfo::Ids elementIds = featureCollectionPtr->GetElementIds();
		for (imtbase::ICollectionInfo::Id& elementId : elementIds){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (featureCollectionPtr->GetObjectData(elementId, dataPtr)){
				const imtlic::CFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::CFeatureInfo*>(dataPtr.GetPtr());
				if (featureInfoPtr != nullptr){
					imtbase::CTreeItemModel featureRepresentationModel;
					QString errorMessage;
					bool ok = CreateRepresentationModelFromFeatureInfo(*featureInfoPtr, featureRepresentationModel, languageId, errorMessage);
					if (ok){
						QJsonObject itemObj;
						QString jsonStr = featureRepresentationModel.ToJson();
						QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
						if (doc.isObject()){
							itemObj = doc.object();
						}
						else if (doc.isArray() && !doc.array().isEmpty()){
							itemObj = doc.array().first().toObject();
						}
						itemObj.insert(QStringLiteral("Id"), QJsonValue::fromVariant(elementId));
						dataArray.append(itemObj);
					}
				}
			}
		}
	}

	resultObj.insert(QStringLiteral("data"), dataArray);
	return resultObj;
}


} // namespace imtlicgql


