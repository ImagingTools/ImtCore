#include <imtauthgui/CContactInfoViewDelegateComp.h>


// ImtCore includes
#include <imtauth/IContactInfo.h>


namespace imtauthgui
{


// public methods

// reimplemented (imtgui::ICollectionViewDelegate)

bool CContactInfoViewDelegateComp::GetSummaryInformation(
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
			if (informationId == QByteArray("EMail")){
				summaryInformation.text = metaInfoPtr->GetMetaInfo(imtauth::IContactInfo::MIT_MAIL).toString();
				summaryInformation.sortValue = summaryInformation.text;
				objectMetaInfo.append(summaryInformation);
			}
			else if (informationId == QByteArray("Birthday")){
				QDate birthday = metaInfoPtr->GetMetaInfo(imtauth::IContactInfo::MIT_BIRTHDAY).toDate();
				QLocale locale;
				summaryInformation.text = locale.toString(birthday, QLocale::ShortFormat);
				summaryInformation.sortValue = summaryInformation.text;
				objectMetaInfo.append(summaryInformation);
			}
			else if (informationId == QByteArray("FirstName")){
				summaryInformation.text = metaInfoPtr->GetMetaInfo(imtauth::IContactInfo::MIT_FIRST_NAME).toString();
				summaryInformation.sortValue = summaryInformation.text;
				objectMetaInfo.append(summaryInformation);
			}
			else if (informationId == QByteArray("LastName")){
				summaryInformation.text = metaInfoPtr->GetMetaInfo(imtauth::IContactInfo::MIT_LAST_NAME).toString();
				summaryInformation.sortValue = summaryInformation.text;
				objectMetaInfo.append(summaryInformation);
			}
			else if (informationId == QByteArray("Nickname")){
				summaryInformation.text = metaInfoPtr->GetMetaInfo(imtauth::IContactInfo::MIT_NICKNAME).toString();
				summaryInformation.sortValue = summaryInformation.text;
				objectMetaInfo.append(summaryInformation);
			}
		}
	}

	return true;
}


bool CContactInfoViewDelegateComp::IsCommandSupported(int commandId) const
{
	if (commandId == CI_DUPLICATE){
		return false;
	}

	return BaseClass::IsCommandSupported(commandId);
}


// protected methods

// reimplemented (imtgui::CObjectCollectionViewDelegate)

void CContactInfoViewDelegateComp::SetupSummaryInformation()
{
	m_summaryInformationTypes.ResetData();
	m_summaryInformationHeaders.clear();

	m_summaryInformationTypes.InsertItem("EMail", tr("E-Mail"), "");
	m_summaryInformationHeaders["EMail"] = HeaderInfo(true, HeaderInfo::IF_FILTERABLE | HeaderInfo::IF_SORTABLE);

	m_summaryInformationTypes.InsertItem(QByteArray("Birthday"), tr("Birthday"), "");
	m_summaryInformationHeaders["Birthday"] = HeaderInfo(false);

	m_summaryInformationTypes.InsertItem(QByteArray("FirstName"), tr("First Name"), "");
	m_summaryInformationHeaders["FirstName"] = HeaderInfo(false, HeaderInfo::IF_FILTERABLE | HeaderInfo::IF_SORTABLE);

	m_summaryInformationTypes.InsertItem(QByteArray("LastName"), tr("Last Name"), "");
	m_summaryInformationHeaders["LastName"] = HeaderInfo(false, HeaderInfo::IF_FILTERABLE | HeaderInfo::IF_SORTABLE);

	m_summaryInformationTypes.InsertItem(QByteArray("Nickname"), tr("Nickname"), "");
	m_summaryInformationHeaders["Nickname"] = HeaderInfo(false, HeaderInfo::IF_FILTERABLE | HeaderInfo::IF_SORTABLE);
}


} // namespace imtauthgui


