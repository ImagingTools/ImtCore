#include <imtlic/CProductInstanceMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtlic/ILicenseInstance.h>
#include <imtlic/IProductInstanceInfo.h>
#include <imtlic/IProductInstanceInfoProvider.h>
#include <imtlic/CProductInstanceMetaInfo.h>


namespace imtlic
{


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)

bool CProductInstanceMetaInfoCreatorComp::CreateMetaInfo(
			const istd::IChangeable* dataPtr,
			const QByteArray& typeId,
			MetaInfoPtr& metaInfoPtr) const
{
	if (typeId != *m_objectTypeIdAttrPtr){
		return false;
	}

	metaInfoPtr.SetPtr(new imod::TModelWrap<CProductInstanceMetaInfo>);

	if (dataPtr == nullptr){
		return true;
	}

	const imtlic::IProductInstanceInfo* productInstancePtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(dataPtr);
	if (productInstancePtr == nullptr){
		return false;
	}

	QString retVal;

	imtbase::ICollectionInfo::Ids ids = productInstancePtr->GetLicenseInstances().GetElementIds();
	for (imtbase::ICollectionInfo::Id id : ids){
		const imtlic::ILicenseInstance* licenseInstancePtr = productInstancePtr->GetLicenseInstance(id);
		QDateTime expirationDate = licenseInstancePtr->GetExpiration();
		QString licenseName = licenseInstancePtr->GetLicenseName();
		QString expirationDateText = QObject::tr("Unlimited");
		if (expirationDate.isValid()){
			expirationDateText = expirationDate.toString("dd.MM.yyyy");
		}

		retVal +=  licenseName + QObject::tr(" (Valid until: %1)").arg(expirationDateText) + "\n";
	}

	retVal.chop(1);

	metaInfoPtr->SetMetaInfo(IProductInstanceInfoProvider::MIT_LICENSE_INSTANCE_INFO_LIST, retVal);
	metaInfoPtr->SetMetaInfo(IProductInstanceInfo::MIT_PRODUCT_INSTANCE_ID, productInstancePtr->GetProductInstanceId());
	metaInfoPtr->SetMetaInfo(IProductInstanceInfo::MIT_CUSTOMER_NAME, productInstancePtr->GetCustomerId());

	return true;
}


} // namespace imtlic


