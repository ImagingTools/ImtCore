// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CFeaturePackageRepresentationControllerComp.h>

// Qt includes
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>


// ImtCore includes
#include <imtlic/CFeatureContainer.h>


namespace imtserverapp
{


// public methods

// reimplemented (imtrest::CObjectRepresentationControllerCompBase)

bool CFeaturePackageRepresentationControllerComp::GetRepresentationFromValue(
			const istd::IChangeable& dataModel,
			QJsonObject& representation,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	const imtlic::CFeatureContainer* packagePtr = dynamic_cast<const imtlic::CFeatureContainer*>(&dataModel);
	Q_ASSERT(packagePtr != nullptr);

	QJsonArray itemsArray;
	QByteArrayList featureCollectionIds = packagePtr->GetFeatureList().GetElementIds().toList();
	for (const QByteArray& featureCollectionId : featureCollectionIds){
		const imtlic::CFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::CFeatureInfo*>(packagePtr->GetFeatureInfo(featureCollectionId).GetPtr());
		if (featureInfoPtr != nullptr){
			QJsonObject featureModel;

			QString errorMessage;
			bool ok = CreateRepresentationModelFromFeatureInfo(*featureInfoPtr, featureModel, errorMessage);
			if (!ok){
				SendErrorMessage(0, errorMessage, "CObjectRepresentationControllerCompBase");

				return false;
			}

			featureModel.insert(QStringLiteral("Id"), QString::fromUtf8(featureCollectionId));
			itemsArray.append(featureModel);
		}
	}

	representation.insert(QStringLiteral("items"), itemsArray);
	return true;
}


// reimplemented (IRepresentationController)

bool CFeaturePackageRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const imtlic::CFeatureContainer* featureContainerPtr = dynamic_cast<const imtlic::CFeatureContainer*>(&dataModel);
	return featureContainerPtr != nullptr;
}


bool CFeaturePackageRepresentationControllerComp::GetDataModelFromRepresentation(
			const QJsonObject& /*representation*/,
			istd::IChangeable& /*dataModel*/) const
{
	return false;
}


bool CFeaturePackageRepresentationControllerComp::GetRepresentationFromDataModel(const istd::IChangeable& dataModel, QJsonObject& representation, const iprm::IParamsSet* paramsPtr) const
{
	return GetRepresentationFromValue(dataModel, representation, paramsPtr);
}


// protected methods

bool CFeaturePackageRepresentationControllerComp::CreateRepresentationModelFromFeatureInfo(
			const imtlic::CFeatureInfo& featureInfo,
			QJsonObject& representationModel,
			QString& errorMessage) const
{
	QByteArray featureId = featureInfo.GetFeatureId();

	representationModel.insert(QStringLiteral("FeatureId"), QString::fromUtf8(featureId));
	representationModel.insert(QStringLiteral("FeatureName"), featureInfo.GetFeatureName());
	representationModel.insert(QStringLiteral("Optional"), featureInfo.IsOptional());
	representationModel.insert(QStringLiteral("FeatureDescription"), featureInfo.GetFeatureDescription());
	representationModel.insert(QStringLiteral("Dependencies"), QString(featureInfo.GetDependencies().join(';')));

	const imtlic::IFeatureInfo::FeatureInfoList& subFeatures = featureInfo.GetSubFeatures();
	if (!subFeatures.isEmpty()){
		QJsonArray childModelArray;

		for (int i = 0; i < subFeatures.count(); i++){
			imtlic::IFeatureInfoSharedPtr featureInfoPtr = subFeatures.at(i);
			if (!featureInfoPtr.IsValid()){
				errorMessage = QString("Unable to create representation model for invalid subfeature. Parent feature id: %1.").arg(qPrintable(featureId));
				return false;
			}

			const imtlic::CFeatureInfo* subFeatureInfoPtr = dynamic_cast<const imtlic::CFeatureInfo*>(featureInfoPtr.GetPtr());
			Q_ASSERT(subFeatureInfoPtr != nullptr);

			QJsonObject subFeatureRepresentationModel;
			bool ok = CreateRepresentationModelFromFeatureInfo(*subFeatureInfoPtr, subFeatureRepresentationModel, errorMessage);
			if (!ok){
				return false;
			}

			childModelArray.append(subFeatureRepresentationModel);
		}
		representationModel.insert(QStringLiteral("ChildModel"), childModelArray);
	}

	return true;
}


} // namespace imtserverapp
