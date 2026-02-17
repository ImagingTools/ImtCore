// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtlic/CFeatureMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtlic/IFeatureInfo.h>


namespace imtlic
{


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)

bool CFeatureMetaInfoCreatorComp::CreateMetaInfo(
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

	const imtlic::IFeatureInfo* featureInfo = dynamic_cast<const imtlic::IFeatureInfo*>(dataPtr);
	if (featureInfo == nullptr){
		return false;
	}

	metaInfoPtr->SetMetaInfo(IFeatureInfo::MIT_FEATURE_ID, featureInfo->GetFeatureId());
	metaInfoPtr->SetMetaInfo(IFeatureInfo::MIT_FEATURE_NAME, featureInfo->GetFeatureName());
	metaInfoPtr->SetMetaInfo(IFeatureInfo::MIT_FEATURE_DESCRIPTION, featureInfo->GetFeatureDescription());
	metaInfoPtr->SetMetaInfo(IFeatureInfo::MIT_IS_OPTIONAL, featureInfo->IsOptional());
	metaInfoPtr->SetMetaInfo(IFeatureInfo::MIT_IS_PERMISSION, featureInfo->IsPermission());

	return true;
}


// public methods of embedded class MetaInfo

QString CFeatureMetaInfoCreatorComp::MetaInfo::GetMetaInfoName(int metaInfoType) const
{
	switch (metaInfoType){
	case IFeatureInfo::MIT_FEATURE_ID:
		return QObject::tr("Feature-ID");
	case IFeatureInfo::MIT_FEATURE_NAME:
		return QObject::tr("Feature Name");
	case IFeatureInfo::MIT_FEATURE_DESCRIPTION:
		return QObject::tr("Feature Description");
	case IFeatureInfo::MIT_IS_OPTIONAL:
		return QObject::tr("Is Optional");
	case IFeatureInfo::MIT_IS_PERMISSION:
		return QObject::tr("Is Permission");
	}
	
	return BaseClass::GetMetaInfoName(metaInfoType);
}


} // namespace imtlic


