#include <imtauthgui/CPersonInfoViewDelegateComp.h>


// ImtCore includes
#include <imtauth/IPersonInfo.h>


namespace imtauthgui
{


// public methods

// reimplemented (imtgui::ICollectionViewDelegate)

imtgui::ICollectionViewDelegate::SummaryInformation CPersonInfoViewDelegateComp::GetSummaryInformation(const QByteArray& objectId, const QByteArray& informationId) const
{
	SummaryInformation result;

	if (m_collectionPtr != nullptr){
		imtbase::IObjectCollection::MetaInfoPtr metaInfoPtr;
		if (m_collectionPtr->GetDataMetaInfo(objectId, metaInfoPtr)){
			if (informationId == QByteArray("Gender")){
				int genderType = metaInfoPtr->GetMetaInfo(imtauth::IPersonInfo::MIT_GENDER_TYPE).toInt();
				switch (genderType){
				case imtauth::IPersonInfo::GenderType::GT_FEMALE:
					result.text = tr("Female");
					break;
				case imtauth::IPersonInfo::GenderType::GT_MALE:
					result.text = tr("Male");
					break;
				default:
					result.text = tr("Diverse");
				};

				result.sortValue = result.text;
			}
			else if (informationId == QByteArray("Birthday")){
				QDate birthday = metaInfoPtr->GetMetaInfo(imtauth::IPersonInfo::MIT_BIRTHDAY).toDate();
				result.text = birthday.toString(Qt::DateFormat::SystemLocaleDate);
				result.sortValue = result.text;
			}
			else if (informationId == QByteArray("FirstName")){
				result.text = metaInfoPtr->GetMetaInfo(imtauth::IPersonInfo::MIT_FIRST_NAME).toString();
				result.sortValue = result.text;
			}
			else if (informationId == QByteArray("LastName")){
				result.text = metaInfoPtr->GetMetaInfo(imtauth::IPersonInfo::MIT_LAST_NAME).toString();
				result.sortValue = result.text;
			}
			else if (informationId == QByteArray("Nickname")){
				result.text = metaInfoPtr->GetMetaInfo(imtauth::IPersonInfo::MIT_NICKNAME).toString();
				result.sortValue = result.text;
			}
		}
	}

	return result;
}


bool CPersonInfoViewDelegateComp::IsCommandSupported(int commandId) const
{
	if (commandId == CI_DUPLICATE){
		return false;
	}

	return BaseClass::IsCommandSupported(commandId);
}


// protected methods

// reimplemented (imtgui::CObjectCollectionViewDelegate)

void CPersonInfoViewDelegateComp::SetupSummaryInformation()
{
	m_summaryInformationTypes.ResetData();
	m_summaryInformationHeaders.clear();

	m_summaryInformationTypes.InsertItem("Gender", tr("Gender"), "");
	m_summaryInformationHeaders["Gender"] = HeaderInfo(true);

	m_summaryInformationTypes.InsertItem(QByteArray("Birthday"), tr("Birthday"), "");
	m_summaryInformationHeaders["Birthday"] = HeaderInfo(false);

	m_summaryInformationTypes.InsertItem(QByteArray("FirstName"), tr("First Name"), "");
	m_summaryInformationHeaders["FirstName"] = HeaderInfo(false);

	m_summaryInformationTypes.InsertItem(QByteArray("LastName"), tr("Last Name"), "");
	m_summaryInformationHeaders["LastName"] = HeaderInfo(false);

	m_summaryInformationTypes.InsertItem(QByteArray("Nickname"), tr("Nickname"), "");
	m_summaryInformationHeaders["Nickname"] = HeaderInfo(false);
}


} // namespace imtauthgui


