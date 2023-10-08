#include <imtauthgui/CAccountInfoViewDelegateComp.h>


// ImtCore includes
#include <imtauth/IAccountInfo.h>


namespace imtauthgui
{


// public methods

// reimplemented (imtgui::ICollectionViewDelegate)

bool CAccountInfoViewDelegateComp::GetSummaryInformation(
			const QByteArray& objectId,
			const QVector<QByteArray> fieldIds,
			ObjectMetaInfo& objectMetaInfo) const
{
	if (m_collectionPtr == nullptr){
		return false;
	}
	idoc::MetaInfoPtr metaInfoPtr = m_collectionPtr->GetDataMetaInfo(objectId);
	for (const QByteArray& informationId: fieldIds){
		SummaryInformation summaryInformation;
		summaryInformation.infoId = informationId;
		if (metaInfoPtr.IsValid()){
			if (informationId == QByteArray("AccountName")){
				summaryInformation.text = metaInfoPtr->GetMetaInfo(imtauth::IAccountInfo::MIT_ACCOUNT_NAME).toString();
				summaryInformation.sortValue = summaryInformation.text;
				objectMetaInfo.append(summaryInformation);
			}
			else if (informationId == QByteArray("Email")){
				summaryInformation.text = metaInfoPtr->GetMetaInfo(imtauth::IAccountInfo::MIT_CONTACT_EMAIL).toString();
				summaryInformation.sortValue = summaryInformation.text;
				objectMetaInfo.append(summaryInformation);
			}
			else if (informationId == QByteArray("AccountType")){
				summaryInformation.text = metaInfoPtr->GetMetaInfo(imtauth::IAccountInfo::MIT_ACCOUNT_TYPE).toString();
				summaryInformation.sortValue = summaryInformation.text;
				objectMetaInfo.append(summaryInformation);
			}
			else if (informationId == QByteArray("AccountDesciption")){
				summaryInformation.text = metaInfoPtr->GetMetaInfo(imtauth::IAccountInfo::MIT_ACCOUNT_DESCRIPTION).toString();
				summaryInformation.sortValue = summaryInformation.text;
				objectMetaInfo.append(summaryInformation);
			}
		}
	}

	return true;
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

	m_summaryInformationTypes.InsertItem(QByteArray("AccountName"), tr("Account Name"), "");
	m_summaryInformationHeaders["AccountName"] = HeaderInfo(false, HeaderInfo::IF_FILTERABLE | HeaderInfo::IF_SORTABLE);

	m_summaryInformationTypes.InsertItem(QByteArray("Email"), tr("E-Mail"), "");
	m_summaryInformationHeaders["Email"] = HeaderInfo(false, HeaderInfo::IF_FILTERABLE | HeaderInfo::IF_SORTABLE);

	m_summaryInformationTypes.InsertItem(QByteArray("AccountType"), tr("Account Type"), "");
	m_summaryInformationHeaders["AccountType"] = HeaderInfo(false, HeaderInfo::IF_FILTERABLE | HeaderInfo::IF_SORTABLE);

	m_summaryInformationTypes.InsertItem(QByteArray("AccountDescription"), tr("Account Description"), "");
	m_summaryInformationHeaders["AccountDescription"] = HeaderInfo(false, HeaderInfo::IF_FILTERABLE | HeaderInfo::IF_SORTABLE);
}


} // namespace imtauthgui


