#include <controlsgallerygql/CContactInfoMetaInfoDelegateComp.h>


// ImtCore includes
#include <imtauth/IContactInfo.h>
#include <imtdbgql/TSdlBasedMetaInfoDelegateImpl.h>


namespace controlsgallerygql
{


// public methods

// reimplemented (imtdb::IJsonBasedMetaInfoDelegate)

bool CContactInfoMetaInfoDelegateComp::ToJsonRepresentation(
			const idoc::IDocumentMetaInfo& metaInfo,
			QByteArray& json,
			const QByteArray& typeId) const
{
	return imtdbgql::TSdlBasedMetaInfoDelegateImpl<sdl::V1_0::controlsgallery::CContactInfoItemData>::ToJsonRepresentation(
			*this,
			metaInfo,
			json,
			typeId);
}


bool CContactInfoMetaInfoDelegateComp::FromJsonRepresentation(
			const QByteArray& json,
			idoc::IDocumentMetaInfo& metaInfo,
			const QByteArray& typeId) const
{
	return imtdbgql::TSdlBasedMetaInfoDelegateImpl<sdl::V1_0::controlsgallery::CContactInfoItemData>::FromJsonRepresentation(
			*this,
			json,
			metaInfo,
			typeId);
}


// protected methods

// reimplemented (imtdbgql::TSdlBasedMetaInfoDelegate<sdl::V1_0::controlsgallery::CContactInfoItemData>)

bool CContactInfoMetaInfoDelegateComp::FillRepresentation(
			sdl::V1_0::controlsgallery::CContactInfoItemData& metaInfoRepresentation,
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
			const sdl::V1_0::controlsgallery::CContactInfoItemData& metaInfoRepresentation,
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


