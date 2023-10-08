#include <imtlicgui/CFeatureCollectionViewDelegateComp.h>


// ImtCore includes
#include <imtlic/IFeatureInfo.h>


namespace imtlicgui
{


// public methods

// reimplemented (imtgui::ICollectionViewDelegate)

bool CFeatureCollectionViewDelegateComp::GetSummaryInformation(
			const QByteArray& objectId,
			const QVector<QByteArray> fieldIds,
			ObjectMetaInfo& objectMetaInfo)const
{
	if (m_collectionPtr == nullptr){
		return false;
	}
	const imtlic::IFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::IFeatureInfo*>(m_collectionPtr->GetObjectPtr(objectId));

	for (const QByteArray& informationId: fieldIds){
		SummaryInformation summaryInformation;
		summaryInformation.infoId = informationId;
		if (featureInfoPtr != nullptr){
			if (informationId == QByteArray("Name")){
				summaryInformation.text = featureInfoPtr->GetFeatureName();
				summaryInformation.sortValue = summaryInformation.text;
				objectMetaInfo.append(summaryInformation);
			}
			else if (informationId == QByteArray("Id")){
				summaryInformation.text = featureInfoPtr->GetFeatureId();
				summaryInformation.sortValue = summaryInformation.text;
				objectMetaInfo.append(summaryInformation);
			}
			else if (informationId == QByteArray("Description")){
				summaryInformation.text = m_collectionPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();
				summaryInformation.sortValue = summaryInformation.text;
				objectMetaInfo.append(summaryInformation);
			}
		}
		else{
			summaryInformation.text = tr("Wrong object type");
			summaryInformation.sortValue = summaryInformation.text;
			objectMetaInfo.append(summaryInformation);
		}
	}

	return true;
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
	m_summaryInformationHeaders["Name"] = HeaderInfo(false, HeaderInfo::IF_FILTERABLE | HeaderInfo::IF_SORTABLE);

	m_summaryInformationTypes.InsertItem("Id", tr("Feature-ID"), "");//изменил FeatureId на Id
	m_summaryInformationHeaders["Id"] = HeaderInfo(true, HeaderInfo::IF_FILTERABLE | HeaderInfo::IF_SORTABLE);

	m_summaryInformationTypes.InsertItem("Description", tr("Description"), "");
	m_summaryInformationHeaders["Description"] = HeaderInfo(false, HeaderInfo::IF_FILTERABLE | HeaderInfo::IF_SORTABLE);
}


} // namespace aculainspgui


