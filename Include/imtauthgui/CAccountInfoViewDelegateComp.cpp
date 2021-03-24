#include <imtauthgui/CAccountInfoViewDelegateComp.h>


// ImtCore includes
#include <imtauth/IAccountInfo.h>


namespace imtauthgui
{


// public methods

// reimplemented (imtgui::ICollectionViewDelegate)

imtgui::ICollectionViewDelegate::SummaryInformation CAccountInfoViewDelegateComp::GetSummaryInformation(const QByteArray& objectId, const QByteArray& informationId) const
{
	SummaryInformation result;

	if (m_collectionPtr != nullptr){
		imtbase::IObjectCollection::MetaInfoPtr metaInfoPtr;
		if (m_collectionPtr->GetDataMetaInfo(objectId, metaInfoPtr)){
			if (informationId == QByteArray("Contact")){
				result.text = metaInfoPtr->GetMetaInfo(imtauth::IAccountInfo::MIT_CONTACT_EMAIL).toString();
				result.sortValue = result.text;
			}
			else if (informationId == QByteArray("AccountType")){
				int type = metaInfoPtr->GetMetaInfo(imtauth::IAccountInfo::MIT_ACCOUNT_TYPE).toInt();

				switch (type){
				case imtauth::IAccountInfo::AT_PERSON:
					result.text = tr("Person");
					result.sortValue = result.text;
					break;
				case imtauth::IAccountInfo::AT_COMPANY:
					result.text = tr("Company");
					result.sortValue = result.text;
					break;
				default:
					result.text = tr("UNKNOWN");
					result.sortValue = result.text;
					break;
				}
			}
			else if (informationId == QByteArray("AccountName")){
				result.text = metaInfoPtr->GetMetaInfo(imtauth::IAccountInfo::MIT_ACCOUNT_NAME).toString();
				result.sortValue = result.text;
			}
			else if (informationId == QByteArray("AccountDesciption")){
				result.text = metaInfoPtr->GetMetaInfo(imtauth::IAccountInfo::MIT_ACCOUNT_DESCRIPTION).toString();
				result.sortValue = result.text;
			}
		}
	}

	return result;
}


bool CAccountInfoViewDelegateComp::IsCommandSupported(int commandId) const
{
	if (commandId == CI_DUPLICATE){
		return false;
	}

	return BaseClass::IsCommandSupported(commandId);
}


// protected methods

// reimplemented (imtgui::CObjectCollectionViewDelegate)

void CAccountInfoViewDelegateComp::SetupSummaryInformation()
{
	m_summaryInformationTypes.ResetData();
	m_summaryInformationHeaders.clear();

	m_summaryInformationTypes.InsertItem(QByteArray("Contact"), tr("Contact EMail"), "");
	m_summaryInformationHeaders["Contact"] = HeaderInfo(false);

	m_summaryInformationTypes.InsertItem(QByteArray("AccountType"), tr("Account Type"), "");
	m_summaryInformationHeaders["AccountType"] = HeaderInfo(false);

	m_summaryInformationTypes.InsertItem(QByteArray("AccountName"), tr("Account Name"), "");
	m_summaryInformationHeaders["AccountName"] = HeaderInfo(false);

	m_summaryInformationTypes.InsertItem(QByteArray("AccountDesciption"), tr("Account Desciption"), "");
	m_summaryInformationHeaders["AccountDesciption"] = HeaderInfo(false);
}


} // namespace imtauthgui


