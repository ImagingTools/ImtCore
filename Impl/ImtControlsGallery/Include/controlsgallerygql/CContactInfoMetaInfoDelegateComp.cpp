#include <controlsgallerygql/CContactInfoMetaInfoDelegateComp.h>


// ImtCore includes
#include <imtauth/IContactInfo.h>


namespace controlsgallerygql
{


// protected methods

// reimplemented (imtdbgql::TSdlBasedMetaInfoDelegate<sdl::controlsgallery::ContactInfos::CContactInfoItemData::V1_0>)

bool CContactInfoMetaInfoDelegateComp::FillRepresentation(
			sdl::controlsgallery::ContactInfos::CContactInfoItemData::V1_0& metaInfoRepresentation,
			const idoc::IDocumentMetaInfo& metaInfo,
			const QByteArray& /*typeId*/) const
{
	QString firstName = metaInfo.GetMetaInfo(imtauth::IContactInfo::MIT_FIRST_NAME).toString();
	metaInfoRepresentation.firstName = firstName;

	QString lastName = metaInfo.GetMetaInfo(imtauth::IContactInfo::MIT_LAST_NAME).toString();
	metaInfoRepresentation.lastName = lastName;

	QString mail = metaInfo.GetMetaInfo(imtauth::IContactInfo::MIT_MAIL).toString();
	metaInfoRepresentation.email = mail;

	return true;
}


bool CContactInfoMetaInfoDelegateComp::FillMetaInfo(
			idoc::IDocumentMetaInfo& metaInfo,
			const sdl::controlsgallery::ContactInfos::CContactInfoItemData::V1_0& metaInfoRepresentation,
			const QByteArray& /*typeId*/) const
{
	if (metaInfoRepresentation.firstName){
		metaInfo.SetMetaInfo(imtauth::IContactInfo::MIT_FIRST_NAME, *metaInfoRepresentation.firstName);
	}

	if (metaInfoRepresentation.lastName){
		metaInfo.SetMetaInfo(imtauth::IContactInfo::MIT_LAST_NAME, *metaInfoRepresentation.lastName);
	}

	if (metaInfoRepresentation.email){
		metaInfo.SetMetaInfo(imtauth::IContactInfo::MIT_MAIL, *metaInfoRepresentation.email);
	}

	return true;
}


} // namespace controlsgallerygql


