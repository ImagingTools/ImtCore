#include <imtlicgql/CLicenseCollectionControllerComp.h>


// ImtCore includes
#include <imtlic/ILicenseDefinition.h>


namespace imtlicgql
{


// protected methods

// reimplemented (imtgql::CObjectCollectionControllerCompBase)

bool CLicenseCollectionControllerComp::SetupGqlItem(
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

	if (!informationIds.isEmpty() && m_objectCollectionCompPtr.IsValid()){
		QByteArray collectionId = objectCollectionIterator->GetObjectId();

		imtlic::ILicenseDefinition* licenseInfoPtr = nullptr;
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (objectCollectionIterator->GetObjectData(dataPtr)){
			licenseInfoPtr = dynamic_cast<imtlic::ILicenseDefinition*>(dataPtr.GetPtr());
		}

		if (licenseInfoPtr == nullptr){
			return false;
		}

		for (const QByteArray& informationId : informationIds){
			QVariant elementInformation;

			if(informationId == "Id"){
				elementInformation = collectionId;
			}
			else if(informationId == "LicenseId"){
				elementInformation = licenseInfoPtr->GetLicenseId();
			}
			else if(informationId == "LicenseName"){
				elementInformation = licenseInfoPtr->GetLicenseName();
			}
			else if(informationId == "LicenseDescription"){
				elementInformation = licenseInfoPtr->GetLicenseDescription();
			}
			else if(informationId == "Added"){
				QDateTime addedTime =  objectCollectionIterator->GetElementInfo("Added").toDateTime();
				elementInformation = addedTime.toString("dd.MM.yyyy hh:mm:ss");
			}
			else if(informationId == "LastModified"){
				QDateTime lastTime =  objectCollectionIterator->GetElementInfo("LastModified").toDateTime();
				elementInformation = lastTime.toString("dd.MM.yyyy hh:mm:ss");
			}
			else if(informationId == "ProductId"){
				elementInformation = objectCollectionIterator->GetElementInfo("ProductId");
			}
			else if(informationId == "ProductUuid"){
				elementInformation = licenseInfoPtr->GetProductId();
			}
			else if(informationId == "ParentLicenses"){
				elementInformation = licenseInfoPtr->GetDependencies().join(';');
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


