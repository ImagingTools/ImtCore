#include <controlsgallerygql/CContactInfoMetaInfoDelegateComp.h>


#include <QJsonObject>

// ImtCore includes
#include <imtauth/IContactInfo.h>


namespace controlsgallerygql
{


// public methods

// reimplemented (imtdb::IJsonBasedMetaInfoDelegate)

bool CContactInfoMetaInfoDelegateComp::ToJsonRepresentation(
			const idoc::IDocumentMetaInfo& metaInfo,
			QByteArray& json,
			const QByteArray& typeId) const
{
	return BaseClass2::ToJsonRepresentation(metaInfo, json, typeId);
}


bool CContactInfoMetaInfoDelegateComp::FromJsonRepresentation(
			const QByteArray& json,
			idoc::IDocumentMetaInfo& metaInfo,
			const QByteArray& typeId) const
{
	return BaseClass2::FromJsonRepresentation(json, metaInfo, typeId);
}


// protected methods

// reimplemented (imtdb::CJsonBasedMetaInfoDelegateComp)

bool CContactInfoMetaInfoDelegateComp::FillRepresentation(
			QJsonObject& representation,
			const idoc::IDocumentMetaInfo& metaInfo,
			const QByteArray& typeId) const
{
	Q_UNUSED(typeId);

	QString firstName = metaInfo.GetMetaInfo(imtauth::IContactInfo::MIT_FIRST_NAME).toString();
	representation["FirstName"] = firstName;

	QString lastName = metaInfo.GetMetaInfo(imtauth::IContactInfo::MIT_LAST_NAME).toString();
	representation["LastName"] = lastName;

	QString mail = metaInfo.GetMetaInfo(imtauth::IContactInfo::MIT_MAIL).toString();
	representation["Email"] = mail;

	return true;
}


bool CContactInfoMetaInfoDelegateComp::FillMetaInfo(
			idoc::IDocumentMetaInfo& metaInfo,
			const QJsonObject& representation,
			const QByteArray& typeId) const
{
	Q_UNUSED(typeId);

	if (representation.contains("FirstName")){
		metaInfo.SetMetaInfo(imtauth::IContactInfo::MIT_FIRST_NAME, representation.value("FirstName"));
	}

	if (representation.contains("LastName")){
		metaInfo.SetMetaInfo(imtauth::IContactInfo::MIT_LAST_NAME, representation.value("LastName"));
	}

	if (representation.contains("Email")){
		metaInfo.SetMetaInfo(imtauth::IContactInfo::MIT_MAIL, representation.value("Email"));
	}

	return true;
}


} // namespace controlsgallerygql
