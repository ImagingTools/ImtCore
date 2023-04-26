#include <imtauth/CCompanyInfoMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtauth/ICompanyInfo.h>
#include <imtauth/CAccountInfoMetaInfo.h>


namespace imtauth
{


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)

bool CCompanyInfoMetaInfoCreatorComp::CreateMetaInfo(
			const istd::IChangeable* dataPtr,
			const QByteArray& typeId,
			idoc::MetaInfoPtr& metaInfoPtr) const
{
	if (typeId != *m_objectTypeIdAttrPtr){
		return false;
	}

	metaInfoPtr.SetPtr(new imod::TModelWrap<CAccountInfoMetaInfo>);

	if (dataPtr == nullptr){
		return true;
	}

	const ICompanyInfo* companyInfoPtr = dynamic_cast<const ICompanyInfo*>(dataPtr);
	if (companyInfoPtr == nullptr){
		return false;
	}

//	metaInfoPtr->SetMetaInfo(imtauth::ICompanyInfo::MIT_ACCOUNT_NAME, companyInfoPtr->GetAccountName());
//	metaInfoPtr->SetMetaInfo(imtauth::ICompanyInfo::MIT_ACCOUNT_DESCRIPTION, companyInfoPtr->GetAccountDescription());
//	metaInfoPtr->SetMetaInfo(imtauth::ICompanyInfo::MIT_MAIL, companyInfoPtr->GetMail());
//	metaInfoPtr->SetMetaInfo(imtauth::ICompanyInfo::MIT_COMPANY_NAME, companyInfoPtr->GetCompanyName());

//	const imtauth::CAddress address = companyInfoPtr->GetAddress();

//	metaInfoPtr->SetMetaInfo(imtauth::IAddress::MIT_COUNTRY, address.GetCountry());
//	metaInfoPtr->SetMetaInfo(imtauth::IAddress::MIT_CITY, address.GetCity());
//	metaInfoPtr->SetMetaInfo(imtauth::IAddress::MIT_POSTAL_CODE, address.GetPostalCode());
//	metaInfoPtr->SetMetaInfo(imtauth::IAddress::MIT_STREET, address.GetStreet());

	return true;
}


} // namespace imtauth


