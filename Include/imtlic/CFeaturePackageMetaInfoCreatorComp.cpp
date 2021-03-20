#include <imtlic/CFeaturePackageMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtlic/IFeatureInfo.h>
#include <imtlic/IFeatureInfoProvider.h>


namespace imtlic
{


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)

imtbase::IMetaInfoCreator::TypeIds CFeaturePackageMetaInfoCreatorComp::GetSupportedTypeIds() const
{
	TypeIds retVal;

	retVal.push_back(*m_objectTypeIdAttrPtr);

	return retVal;
}


bool CFeaturePackageMetaInfoCreatorComp::CreateMetaInfo(
			const istd::IChangeable* dataPtr,
			const QByteArray& typeId,
			MetaInfoPtr& metaInfoPtr) const
{
	if (typeId != *m_objectTypeIdAttrPtr){
		return false;
	}

	class MetaInfo: public idoc::CStandardDocumentMetaInfo
	{
	public:
		typedef idoc::CStandardDocumentMetaInfo BaseClass;

		// reimplemented (idoc::IDocumentMetaInfo)
		virtual QString GetMetaInfoName(int metaInfoType) const override
		{
			switch (metaInfoType){
			case imtlic::IFeatureInfoProvider::MIT_FEATURE_INFO_LIST:
				return QObject::tr("Features");
			}

			return BaseClass::GetMetaInfoName(metaInfoType);
		}
	};

	metaInfoPtr.SetPtr(new imod::TModelWrap<MetaInfo>);

	if (dataPtr == nullptr){
		return true;
	}

	const imtlic::IFeatureInfoProvider* featurePckagePtr = dynamic_cast<const imtlic::IFeatureInfoProvider*>(dataPtr);
	if (featurePckagePtr == nullptr){
		return false;
	}

	QString retVal;

	imtbase::ICollectionInfo::Ids ids = featurePckagePtr->GetFeatureList().GetElementIds();
	for (imtbase::ICollectionInfo::Id id : ids){
		const imtlic::IFeatureInfo* featureInfoPtr = featurePckagePtr->GetFeatureInfo(id);
		retVal += featureInfoPtr->GetFeatureName() + "\n";
	}

	retVal.chop(1);

	metaInfoPtr->SetMetaInfo(IFeatureInfoProvider::MIT_FEATURE_INFO_LIST, retVal);

	return true;
}


} // namespace imtlic


