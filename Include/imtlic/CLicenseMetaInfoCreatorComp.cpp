// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlic/CLicenseMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtlic/ILicenseDefinition.h>


namespace imtlic
{


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)

bool CLicenseMetaInfoCreatorComp::CreateMetaInfo(
			const istd::IChangeable* dataPtr,
			const QByteArray& typeId,
			idoc::MetaInfoPtr& metaInfoPtr) const
{
	if (typeId != *m_objectTypeIdAttrPtr){
		return false;
	}

	metaInfoPtr.SetPtr(new imod::TModelWrap<MetaInfo>);

	if (dataPtr == nullptr){
		return true;
	}

	const imtlic::ILicenseDefinition* licenseInfoPtr = dynamic_cast<const imtlic::ILicenseDefinition*>(dataPtr);
	if (licenseInfoPtr == nullptr){
		return false;
	}

	metaInfoPtr->SetMetaInfo(ILicenseDefinition::MIT_LICENSE_ID, licenseInfoPtr->GetLicenseId());
	metaInfoPtr->SetMetaInfo(ILicenseDefinition::MIT_LICENSE_NAME, licenseInfoPtr->GetLicenseName());
	metaInfoPtr->SetMetaInfo(ILicenseDefinition::MIT_LICENSE_DESCRIPTION, licenseInfoPtr->GetLicenseDescription());
	metaInfoPtr->SetMetaInfo(ILicenseDefinition::MIT_PRODUCT_ID, licenseInfoPtr->GetProductId());

	return true;
}


// public methods of embedded class MetaInfo

QString CLicenseMetaInfoCreatorComp::MetaInfo::GetMetaInfoName(int metaInfoType) const
{
	switch (metaInfoType){
	case ILicenseDefinition::MIT_LICENSE_ID:
		return QObject::tr("License-ID");
	case ILicenseDefinition::MIT_LICENSE_NAME:
		return QObject::tr("License Name");
	case ILicenseDefinition::MIT_LICENSE_DESCRIPTION:
		return QObject::tr("License Description");
	case ILicenseDefinition::MIT_PRODUCT_ID:
		return QObject::tr("Product-ID");
	}
	
	return BaseClass::GetMetaInfoName(metaInfoType);
}


} // namespace imtlic


