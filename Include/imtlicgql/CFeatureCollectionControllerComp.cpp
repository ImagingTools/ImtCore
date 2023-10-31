#include <imtlicgql/CFeatureCollectionControllerComp.h>


// ImtCore includes
#include <imtlic/CFeatureInfo.h>


namespace imtlicgql
{


// protected methods

// reimplemented (imtgql::CObjectCollectionControllerCompBase)

bool CFeatureCollectionControllerComp::SetupGqlItem(
			const imtgql::CGqlRequest& gqlRequest,
			imtbase::CTreeItemModel& model,
			int itemIndex,
			const imtbase::IObjectCollectionIterator* objectCollectionIterator,
			QString& errorMessage) const
{
	if (objectCollectionIterator == nullptr){
		return false;
	}

	bool retVal = true;

	QByteArrayList informationIds = GetInformationIds(gqlRequest, "items");

	QByteArray objectId = objectCollectionIterator->GetObjectId();

	if (!informationIds.isEmpty() && m_objectCollectionCompPtr.IsValid()){
		QByteArray json = objectCollectionIterator->GetElementInfo("Document").toByteArray();
		QJsonDocument jsonDocument = QJsonDocument::fromJson(json);

		for (QByteArray informationId : informationIds){
			QVariant elementInformation;

			if(informationId == "Id"){
				elementInformation = objectId;
			}
			else if(informationId == "FeatureId"){
				elementInformation = jsonDocument["FeatureId"].toString();
			}
			else if(informationId == "FeatureName"){
				elementInformation = jsonDocument["FeatureName"].toString();
			}
			else if(informationId == "FeatureDescription"){
				elementInformation = jsonDocument["FeatureDescription"].toString();
			}
			else if(informationId == "Added"){
				QDateTime addedTime =  objectCollectionIterator->GetElementInfo("Added").toDateTime();
				addedTime.setTimeSpec(Qt::UTC);

				elementInformation = addedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
			}
			else if(informationId == "LastModified"){
				QDateTime lastTime =  objectCollectionIterator->GetElementInfo("LastModified").toDateTime();
				lastTime.setTimeSpec(Qt::UTC);

				elementInformation = lastTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
			}

			if (elementInformation.isNull()){
				elementInformation = "";
			}

			retVal = retVal && model.SetData(informationId, elementInformation, itemIndex);
		}

		return true;
	}

	return false;
}


} // namespace imtlicgql


