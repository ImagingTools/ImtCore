#include <imtlic/CFeaturePackageMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtlic/ILicenseInfo.h>
#include <imtlic/IProductLicensingInfo.h>
#include <imtlic/IProductLicensingInfoProvider.h>


namespace imtlic
{


class MetaInfo: public idoc::CStandardDocumentMetaInfo
{
public:
	typedef idoc::CStandardDocumentMetaInfo BaseClass;

	// reimplemented (idoc::IDocumentMetaInfo)
	virtual QString GetMetaInfoName(int metaInfoType) const override
	{
		switch (metaInfoType){
		case IProductLicensingInfoProvider::MIT_LICENSES_INFO_LIST:
			return QObject::tr("Licenses");
		}

		return BaseClass::GetMetaInfoName(metaInfoType);
	}
};


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

	metaInfoPtr.SetPtr(new imod::TModelWrap<MetaInfo>);

	if (dataPtr == nullptr){
		return true;
	}

	return true;
}


} // namespace imtlic


