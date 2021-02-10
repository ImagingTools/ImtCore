#include <imtgui/CLicenseManagerViewDelegateComp.h>


// Qt includes
#include <QtCore/QUuid>

// ImtCore includes
#include <imtbase/CLicenseInfo.h>


namespace imtgui
{


// reimplemented (ICollectionViewDelegate)

void CLicenseManagerViewDelegateComp::UpdateItemSelection(
			const imtbase::ICollectionInfo::Ids& selectedItems,
			const QByteArray& selectedTypeId)
{
	BaseClass2::UpdateItemSelection(selectedItems, selectedTypeId);

	m_editContentsCommand.setEnabled(selectedItems.count() == 1);
}


QByteArray CLicenseManagerViewDelegateComp::CreateNewObject(const QByteArray& typeId, const istd::IChangeable* /*defaultDataPtr*/) const
{
	if (m_collectionPtr != nullptr){
		imtbase::CLicenseInfo licenseInfo;

		if (m_defaultLicenseNameAttrPtr.IsValid()){
			licenseInfo.SetLicenseName(*m_defaultLicenseNameAttrPtr);
		}

		if (m_defaultLicenseIdAttrPtr.IsValid()){
			licenseInfo.SetLicenseId(*m_defaultLicenseIdAttrPtr);
		}

		if (m_defaultPackageIdAttrPtr.IsValid()){
			licenseInfo.SetPackageId(*m_defaultPackageIdAttrPtr);
		}

		return m_collectionPtr->InsertNewObject(typeId, tr("New License"), QString(), &licenseInfo);
	}

	return QByteArray();
}


ICollectionViewDelegate::SummaryInformation CLicenseManagerViewDelegateComp::GetSummaryInformation(const QByteArray& objectId, const QByteArray& informationId) const
{
	SummaryInformation result;

	const imtbase::ILicenseInfo* licenseInfoPtr = dynamic_cast<const imtbase::ILicenseInfo*>(m_collectionPtr->GetObjectPtr(objectId));

	if (licenseInfoPtr != nullptr){
		if (informationId == QByteArray("LicenseId")){
			result.text = licenseInfoPtr->GetLicenseId();
			result.sortValue = result.text;
		}
		else if (informationId == QByteArray("PackageId")){
			result.text = licenseInfoPtr->GetPackageId();
			result.sortValue = result.text;
		}
		else if (informationId == QByteArray("LicenseName")){
			result.text = licenseInfoPtr->GetLicenseName();
			result.sortValue = result.text;
		}
		else if (informationId == QByteArray("Expiration")){
			QDateTime dateTime = licenseInfoPtr->GetExpiration();

			if (dateTime.isValid()){
				result.text = dateTime.date().toString("dd.MM.yyyy");
				result.sortValue = licenseInfoPtr->GetExpiration().toSecsSinceEpoch();
			}
			else{
				result.text = tr("Unlimited");
				result.sortValue = tr("Unlimited");
			}
		}
	}

	return result;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CLicenseManagerViewDelegateComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	SetupSummaryInformation();
}


// reimplemented (CObjectCollectionViewDelegate)

void CLicenseManagerViewDelegateComp::SetupSummaryInformation()
{
	m_summaryInformationTypes.ResetData();
	m_summaryInformationHeaders.clear();

	m_summaryInformationTypes.InsertItem("LicenseName", tr("License Name"), "");
	m_summaryInformationHeaders["LicenseName"] = HeaderInfo(true);

	m_summaryInformationTypes.InsertItem("LicenseId", tr("License-ID"), "");
	m_summaryInformationHeaders["LicenseId"] = HeaderInfo(true);

	m_summaryInformationTypes.InsertItem("PackageId", tr("Package-ID"), "");
	m_summaryInformationHeaders["PackageId"] = HeaderInfo(false);

	m_summaryInformationTypes.InsertItem("Expiration", tr("Expiration"), "");
	m_summaryInformationHeaders["Expiration"] = HeaderInfo(false);
}


} // namespace imtgui


