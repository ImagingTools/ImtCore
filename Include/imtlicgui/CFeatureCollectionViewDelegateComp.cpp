#include <imtlicgui/CFeatureCollectionViewDelegateComp.h>


// ImtCore includes
#include <imtlic/IFeatureInfo.h>


namespace imtlicgui
{


// public methods

// reimplemented (imtgui::ICollectionViewDelegate)

imtgui::ICollectionViewDelegate::SummaryInformation CFeatureCollectionViewDelegateComp::GetSummaryInformation(const QByteArray& objectId, const QByteArray& informationId) const
{
	SummaryInformation retVal;

	if (m_collectionPtr != nullptr){
		const imtlic::IFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::IFeatureInfo*>(m_collectionPtr->GetObjectPtr(objectId));
		if (featureInfoPtr != nullptr){
			if (informationId == QByteArray("Name")){
				retVal.text = featureInfoPtr->GetFeatureName();
				retVal.sortValue = retVal.text;
			}
			else if (informationId == QByteArray("Id")){
				retVal.text = featureInfoPtr->GetFeatureId();
				retVal.sortValue = retVal.text;
			}
			else if (informationId == QByteArray("Description")){
				retVal.text = m_collectionPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();
				retVal.sortValue = retVal.text;
			}
		}
		else{
			retVal.text = tr("Wrong object type");
			retVal.sortValue = retVal.text;
		}
	}

	return retVal;
}


bool CFeatureCollectionViewDelegateComp::IsCommandSupported(int commandId) const
{
	if (commandId == CI_RENAME || commandId == CI_DUPLICATE){
		return false;
	}

	return BaseClass::IsCommandSupported(commandId);
}


// protected methods

// reimplemented (imtgui::CObjectCollectionViewDelegate)

void CFeatureCollectionViewDelegateComp::SetupSummaryInformation()
{
	m_summaryInformationTypes.ResetData();
	m_summaryInformationHeaders.clear();

	m_summaryInformationTypes.InsertItem("Name", tr("Feature Name"), "");//изменил FeatureId на Id
	m_summaryInformationHeaders["Name"] = HeaderInfo(false);

	m_summaryInformationTypes.InsertItem("Id", tr("Feature-ID"), "");//изменил FeatureId на Id
	m_summaryInformationHeaders["Id"] = HeaderInfo(true);

	m_summaryInformationTypes.InsertItem("Description", tr("Description"), "");
	m_summaryInformationHeaders["Description"] = HeaderInfo(false);
}


} // namespace aculainspgui


