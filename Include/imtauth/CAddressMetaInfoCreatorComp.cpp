#include <imtauth/CAddressMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtauth/IAddress.h>


namespace imtauth
{


class MetaInfo: public idoc::CStandardDocumentMetaInfo
{
public:
	typedef idoc::CStandardDocumentMetaInfo BaseClass;

	// reimplemented (idoc::IDocumentMetaInfo)
	virtual QString GetMetaInfoName(int metaInfoType) const override
	{
		switch (metaInfoType){
		case IAddress::MIT_CITY:
			return QObject::tr("City");
		case IAddress::MIT_COUNTRY:
			return QObject::tr("Country");
		case IAddress::MIT_POSTAL_CODE:
			return QObject::tr("Postal Code");
		}

		return BaseClass::GetMetaInfoName(metaInfoType);
	}
};


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)

bool CAddressMetaInfoCreatorComp::CreateMetaInfo(
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

	const IAddress* addressPtr = dynamic_cast<const IAddress*>(dataPtr);
	if (addressPtr == nullptr){
		return false;
	}

	metaInfoPtr->SetMetaInfo(IAddress::MIT_CITY, addressPtr->GetCity());
	metaInfoPtr->SetMetaInfo(IAddress::MIT_COUNTRY, addressPtr->GetCountry());
	metaInfoPtr->SetMetaInfo(IAddress::MIT_POSTAL_CODE, addressPtr->GetPostalCode());

	return true;
}


} // namespace imtauth


