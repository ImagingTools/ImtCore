#include <imtlic/CProductInstanceMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtlic/ILicenseInstance.h>
#include <imtlic/IProductInstanceInfo.h>


namespace imtlic
{


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)

imtbase::IMetaInfoCreator::TypeIds CProductInstanceMetaInfoCreatorComp::GetSupportedTypeIds() const
{
	TypeIds retVal;

	retVal.push_back(*m_objectTypeIdAttrPtr);

	return retVal;
}


bool CProductInstanceMetaInfoCreatorComp::CreateMetaInfo(
			const istd::IChangeable* dataPtr,
			const QByteArray& typeId,
			MetaInfoPtr& metaInfoPtr) const
{
	if (typeId != *m_objectTypeIdAttrPtr){
		return false;
	}

	metaInfoPtr.SetPtr(new imod::TModelWrap<MetaInfo>);

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
		QString expirationDateText = expirationDate.toString("dd.MM.yyyy");
		QString licenseName = licenseInstancePtr->GetLicenseName();
		retVal +=  licenseName + QObject::tr(" (Valid until: %1)").arg(expirationDateText) + "\n";
	}

	retVal.chop(1);

	metaInfoPtr->SetMetaInfo(IProductInstanceInfoProvider::MIT_LICENSE_INSTANCE_INFO_LIST, retVal);

	return true;
}


} // namespace imtlic


