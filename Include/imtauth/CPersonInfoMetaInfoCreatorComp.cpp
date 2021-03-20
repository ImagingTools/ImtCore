#include <imtauth/CPersonInfoMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtauth/IPersonInfo.h>


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
		case IPersonInfo::MIT_GENDER_TYPE:
			return QObject::tr("Gender");
		case IPersonInfo::MIT_BIRTHDAY:
			return QObject::tr("Birthday");
		case IPersonInfo::MIT_FIRST_NAME:
			return QObject::tr("First Name");
		case IPersonInfo::MIT_LAST_NAME:
			return QObject::tr("Last Name");
		case IPersonInfo::MIT_NICKNAME:
			return QObject::tr("Nickname");
		}

		return BaseClass::GetMetaInfoName(metaInfoType);
	}
};


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)

bool CPersonInfoMetaInfoCreatorComp::CreateMetaInfo(
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

	const IPersonInfo* personInfoPtr = dynamic_cast<const IPersonInfo*>(dataPtr);
	if (personInfoPtr == nullptr){
		return false;
	}

	metaInfoPtr->SetMetaInfo(IPersonInfo::MIT_GENDER_TYPE, personInfoPtr->GetGenderType());
	metaInfoPtr->SetMetaInfo(IPersonInfo::MIT_BIRTHDAY, personInfoPtr->GetBirthday());
	metaInfoPtr->SetMetaInfo(IPersonInfo::MIT_FIRST_NAME, personInfoPtr->GetNameField(IPersonInfo::NFT_FIRST_NAME));
	metaInfoPtr->SetMetaInfo(IPersonInfo::MIT_LAST_NAME, personInfoPtr->GetNameField(IPersonInfo::NFT_LAST_NAME));
	metaInfoPtr->SetMetaInfo(IPersonInfo::MIT_NICKNAME, personInfoPtr->GetNameField(IPersonInfo::NFT_NICKNAME));

	return true;
}


} // namespace imtauth


