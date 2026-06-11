#include <controlsgallerygql/CContactInfoMetaInfoDelegateComp.h>


// ImtCore includes
#include <imtauth/IContactInfo.h>


namespace controlsgallerygql
{


// public methods

// reimplemented (imtdb::IJsonBasedMetaInfoDelegate)

bool CContactInfoMetaInfoDelegateComp::ToJsonRepresentation(
			const idoc::IDocumentMetaInfo& metaInfo,
			QByteArray& json,
			const QByteArray& /*typeId*/) const
{
	sdl::V1_0::controlsgallery::CContactInfoItemData representation;

	QString firstName = metaInfo.GetMetaInfo(imtauth::IContactInfo::MIT_FIRST_NAME).toString();
	representation.firstName = firstName;

	QString lastName = metaInfo.GetMetaInfo(imtauth::IContactInfo::MIT_LAST_NAME).toString();
	representation.lastName = lastName;

	QString mail = metaInfo.GetMetaInfo(imtauth::IContactInfo::MIT_MAIL).toString();
	representation.email = mail;

	QJsonObject object;
	if (representation.WriteToJsonObject(object))
	{
		json = QJsonDocument(object).toJson(QJsonDocument::Compact);
		return !json.isEmpty();
	}

	return false;
}


bool CContactInfoMetaInfoDelegateComp::FromJsonRepresentation(
			const QByteArray& json,
			idoc::IDocumentMetaInfo& metaInfo,
			const QByteArray& /*typeId*/) const
{
	QJsonDocument document = QJsonDocument::fromJson(json);
	if (!document.isObject()) {
		return false;
	}

	sdl::V1_0::controlsgallery::CContactInfoItemData representation;
	if (!representation.ReadFromJsonObject(document.object())) {
		return false;
	}

	metaInfo.ResetData();

	if (representation.firstName){
		metaInfo.SetMetaInfo(imtauth::IContactInfo::MIT_FIRST_NAME, *representation.firstName);
	}

	if (representation.lastName){
		metaInfo.SetMetaInfo(imtauth::IContactInfo::MIT_LAST_NAME, *representation.lastName);
	}

	if (representation.email){
		metaInfo.SetMetaInfo(imtauth::IContactInfo::MIT_MAIL, *representation.email);
	}

	return true;
}


} // namespace controlsgallerygql


