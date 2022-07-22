#include <imtauthgui/CContactInfoViewDelegateComp.h>


// ImtCore includes
#include <imtauth/IContactInfo.h>


namespace imtauthgui
{


// public methods

// reimplemented (imtgui::ICollectionViewDelegate)

imtgui::ICollectionViewDelegate::SummaryInformation CContactInfoViewDelegateComp::GetSummaryInformation(const QByteArray& objectId, const QByteArray& informationId) const
{
	SummaryInformation result;

	if (m_collectionPtr != nullptr){
		imtbase::IObjectCollection::MetaInfoPtr metaInfoPtr = m_collectionPtr->GetDataMetaInfo(objectId);
		if (metaInfoPtr.IsValid()){
			if (informationId == QByteArray("EMail")){
				result.text = metaInfoPtr->GetMetaInfo(imtauth::IContactInfo::MIT_MAIL).toString();
				result.sortValue = result.text;
			}
			else if (informationId == QByteArray("Birthday")){
				QDate birthday = metaInfoPtr->GetMetaInfo(imtauth::IContactInfo::MIT_BIRTHDAY).toDate();
				QLocale locale;
				result.text = locale.toString(birthday, QLocale::ShortFormat);
				result.sortValue = result.text;
			}
			else if (informationId == QByteArray("FirstName")){
				result.text = metaInfoPtr->GetMetaInfo(imtauth::IContactInfo::MIT_FIRST_NAME).toString();
				result.sortValue = result.text;
			}
			else if (informationId == QByteArray("LastName")){
				result.text = metaInfoPtr->GetMetaInfo(imtauth::IContactInfo::MIT_LAST_NAME).toString();
				result.sortValue = result.text;
			}
			else if (informationId == QByteArray("Nickname")){
				result.text = metaInfoPtr->GetMetaInfo(imtauth::IContactInfo::MIT_NICKNAME).toString();
				result.sortValue = result.text;
			}
		}
	}

	result.infoId = informationId;

	return result;
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


