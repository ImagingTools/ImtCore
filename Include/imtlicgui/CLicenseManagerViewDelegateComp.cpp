#include <imtlicgui/CLicenseManagerViewDelegateComp.h>


// Qt includes
#include <QtCore/QUuid>

// ImtCore includes
#include <imtlic/CLicenseInfo.h>


namespace imtlicgui
{


// public methods

// reimplemented (imtgui::ICollectionViewDelegate)

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
		imtlic::CLicenseInfo licenseInfo;

		QString objectName;

		if (m_defaultLicenseNameAttrPtr.IsValid()){
			objectName = GetUniqueName(*m_defaultLicenseNameAttrPtr);
		}
		else{
			objectName = GetUniqueName(tr("New License"));
		}

		licenseInfo.SetLicenseName(objectName);

		if (m_defaultLicenseIdAttrPtr.IsValid()){
			licenseInfo.SetLicenseId(*m_defaultLicenseIdAttrPtr);
		}

		return m_collectionPtr->InsertNewObject(typeId, objectName, QString(), &licenseInfo);
	}

	return QByteArray();
}


bool CLicenseManagerViewDelegateComp::RenameObject(const QByteArray& objectId, const QString& newName) const
{
	QString name = GetUniqueName(newName);

	if (BaseClass::RenameObject(objectId, name)){
		imtbase::IObjectCollection::DataPtr dataPtr;
		m_collectionPtr->GetObjectData(objectId, dataPtr);

		imtlic::CLicenseInfo* licenseInfoPtr = dynamic_cast<imtlic::CLicenseInfo*>(dataPtr.GetPtr());
		if (licenseInfoPtr != nullptr){
			QString objectName = m_collectionPtr->GetElementInfo(objectId, imtbase::IObjectCollectionInfo::EIT_NAME).toString();
			licenseInfoPtr->SetLicenseName(objectName);
			m_collectionPtr->SetObjectData(objectId, *licenseInfoPtr);
		}

		return true;
	}

	return false;
}


imtgui::ICollectionViewDelegate::SummaryInformation CLicenseManagerViewDelegateComp::GetSummaryInformation(const QByteArray& objectId, const QByteArray& informationId) const
{
	SummaryInformation result;

	const imtlic::ILicenseInfo* licenseInfoPtr = dynamic_cast<const imtlic::ILicenseInfo*>(m_collectionPtr->GetObjectPtr(objectId));

	if (licenseInfoPtr != nullptr){
		if (informationId == QByteArray("LicenseId")){
			result.text = licenseInfoPtr->GetLicenseId();
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

// reimplemented (imtgui::CObjectCollectionViewDelegate)

void CLicenseManagerViewDelegateComp::OnDuplicateObject(const QByteArray& /*sourceObjectId*/, const QByteArray& destinationObjectId)
{
	imtbase::IObjectCollection::DataPtr dataPtr;
	m_collectionPtr->GetObjectData(destinationObjectId, dataPtr);

	imtlic::CLicenseInfo* licenseInfoPtr = dynamic_cast<imtlic::CLicenseInfo*>(dataPtr.GetPtr());
	if (licenseInfoPtr != nullptr){
		QString objectName = m_collectionPtr->GetElementInfo(destinationObjectId, imtbase::IObjectCollectionInfo::EIT_NAME).toString();
		licenseInfoPtr->SetLicenseName(objectName);
		m_collectionPtr->SetObjectData(destinationObjectId, *licenseInfoPtr);
	}
}


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


// protected slots

//QByteArray CLicenseManagerViewDelegateComp::OnInsert()
//{
//	QByteArray objectId = BaseClass::OnInsert();
//
//	return objectId;
//}
//
//
//void OnDuplicateObject(const QByteArray& sourceObjectId, const QByteArray& destinationObjectId)
//{
//
//}


} // namespace imtlicgui


