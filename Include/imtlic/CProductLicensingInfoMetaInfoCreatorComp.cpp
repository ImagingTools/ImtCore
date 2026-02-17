// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlic/CProductLicensingInfoMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtlic/ILicenseDefinition.h>
#include <imtlic/IProductLicensingInfo.h>
#include <imtlic/IProductLicensingInfoProvider.h>


namespace imtlic
{


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)


bool CProductLicensingInfoMetaInfoCreatorComp::CreateMetaInfo(
			const istd::IChangeable* dataPtr,
			const QByteArray& typeId,
			idoc::MetaInfoPtr& metaInfoPtr) const
{
		if (m_objectTypeIdsAttrPtr.FindValue(typeId) < 0){
		return false;
	}

	metaInfoPtr.SetPtr(new imod::TModelWrap<MetaInfo>);

	if (dataPtr == nullptr){
		return true;
	}

	auto productLicensingInfoPtr = dynamic_cast<const IProductLicensingInfo*>(dataPtr);
	if (productLicensingInfoPtr == nullptr){
		return false;
	}

	QString retVal;

	imtbase::ICollectionInfo::Ids ids = productLicensingInfoPtr->GetLicenseList().GetElementIds();
	for (const imtbase::ICollectionInfo::Id& id : ids){
		istd::TUniqueInterfacePtr<ILicenseDefinition> licenseInfoPtr = productLicensingInfoPtr->GetLicenseInfo(id);
		if (licenseInfoPtr.IsValid()){
			retVal += licenseInfoPtr->GetLicenseName() + " (" + licenseInfoPtr->GetLicenseId() + ")" + "\n";
		}
	}

	retVal.chop(1);
	metaInfoPtr->SetMetaInfo(IProductLicensingInfoProvider::MIT_LICENSES_INFO_LIST, retVal);

	return true;
}


// public methods of embedded class MetaInfo

QString CProductLicensingInfoMetaInfoCreatorComp::MetaInfo::GetMetaInfoName(int metaInfoType) const
{
	switch (metaInfoType){
	case IProductLicensingInfoProvider::MIT_LICENSES_INFO_LIST:
		return QObject::tr("Licenses");
	}

	return BaseClass::GetMetaInfoName(metaInfoType);
}


} // namespace imtlic


