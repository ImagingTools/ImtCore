#include <imtlic/CProductLicensingInfoMetaInfoCreatorComp.h>


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

imtbase::IMetaInfoCreator::TypeIds CProductLicensingInfoMetaInfoCreatorComp::GetSupportedTypeIds() const
{
	TypeIds retVal;

	retVal.push_back(*m_objectTypeIdAttrPtr);

	return retVal;
}


bool CProductLicensingInfoMetaInfoCreatorComp::CreateMetaInfo(
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

	const imtlic::IProductLicensingInfo* productLicensingInfoPtr = dynamic_cast<const imtlic::IProductLicensingInfo*>(dataPtr);
	if (productLicensingInfoPtr == nullptr){
		return false;
	}

	QString retVal;

	imtbase::ICollectionInfo::Ids ids = productLicensingInfoPtr->GetLicenseList().GetElementIds();
	for (imtbase::ICollectionInfo::Id id : ids){
		const imtlic::ILicenseInfo* licenseInfoPtr = productLicensingInfoPtr->GetLicenseInfo(id);
		retVal += licenseInfoPtr->GetLicenseName() + " (" + licenseInfoPtr->GetLicenseId() + ")" + "\n";
	}

	retVal.chop(1);

	metaInfoPtr->SetMetaInfo(IProductLicensingInfoProvider::MIT_LICENSES_INFO_LIST, retVal);

	return true;
}


} // namespace imtlic


