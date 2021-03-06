#include <imtlicgui/CProductInstanceInfoEditorComp.h>


// ACF includes
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtlic/IProductLicensingInfo.h>
#include <imtlic/ILicenseInfo.h>


namespace imtlicgui
{


// public methods

CProductInstanceInfoEditorComp::CProductInstanceInfoEditorComp()
	:m_productCollectionObserver(*this),
	m_licenseCollectionObserver(*this)
{
}


// protected methods

void CProductInstanceInfoEditorComp::OnProductsUpdated(
			const istd::IChangeable::ChangeSet& /*changeSet*/,
			const imtbase::IObjectCollection* /*productCollectionPtr*/)
{
	UpdateProductsCombo();
	UpdateLicensesCombo();
}


void CProductInstanceInfoEditorComp::OnLicensesUpdated(
			const istd::IChangeable::ChangeSet& /*changeSet*/,
			const imtbase::IObjectCollection* /*productCollectionPtr*/)
{
	UpdateLicensesCombo();
}


void CProductInstanceInfoEditorComp::UpdateProductsCombo()
{
	ProductCombo->clear();

	m_licenseCollectionObserver.UnregisterAllObjects();

	imtlic::IProductInstanceInfo* productInstanceInfoPtr = GetObservedObject();
	Q_ASSERT(productInstanceInfoPtr != nullptr);

	QByteArray selectedProductId = productInstanceInfoPtr->GetProductId();

	const imtbase::IObjectCollection* productsCollectionPtr = productInstanceInfoPtr->GetProductDatabase();
	if (productsCollectionPtr != nullptr){
		const imtbase::IObjectCollectionInfo::Ids productIds = productsCollectionPtr->GetElementIds();
		for ( const QByteArray& productId : productIds){
			QString productName = productsCollectionPtr->GetElementInfo(productId, imtbase::ICollectionInfo::EIT_NAME).toString();

			ProductCombo->addItem(productName, productId);

			if (selectedProductId == productId){
				ProductCombo->setCurrentText(productName);

				m_licenseCollectionObserver.RegisterObject(productsCollectionPtr->GetObjectPtr(productId), &CProductInstanceInfoEditorComp::OnLicensesUpdated);
			}
		}
	}

	if (selectedProductId.isEmpty()){
		ProductCombo->setCurrentIndex(-1);
	}
}


void CProductInstanceInfoEditorComp::UpdateLicensesCombo()
{
	LicenseCombo->clear();

	bool licenseSelected = false;

	imtlic::IProductInstanceInfo* productInstanceInfoPtr = GetObservedObject();
	Q_ASSERT(productInstanceInfoPtr != nullptr);

	const imtbase::IObjectCollection* productsCollectionPtr = productInstanceInfoPtr->GetProductDatabase();
	if (productsCollectionPtr != nullptr){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (productsCollectionPtr->GetObjectData(productInstanceInfoPtr->GetProductId(), dataPtr)){
			const imtlic::IProductLicensingInfo* licensingInfoPtr = dynamic_cast<const imtlic::IProductLicensingInfo*>(dataPtr.GetPtr());
			if (licensingInfoPtr != nullptr){
				const imtbase::ICollectionInfo& licenseList = licensingInfoPtr->GetLicenseList();
				const imtbase::IObjectCollectionInfo::Ids licenseCollectionIds = licenseList.GetElementIds();
				for ( const QByteArray& collectionId : licenseCollectionIds){
					const imtlic::ILicenseInfo* licenseInfoPtr = licensingInfoPtr->GetLicenseInfo(collectionId);
					if (licenseInfoPtr != nullptr){
						QString licenseName = licenseInfoPtr->GetLicenseName();
						QByteArray licenseId = licenseInfoPtr->GetLicenseId();

						licenseName += " (" + licenseId + ")";

						LicenseCombo->addItem(licenseName, licenseId);

						const imtbase::ICollectionInfo& currentLicenses = productInstanceInfoPtr->GetLicenseList();
						imtbase::ICollectionInfo::Ids currentLicenseIds = currentLicenses.GetElementIds();

						if (currentLicenseIds.contains(licenseId)){
							LicenseCombo->setCurrentText(licenseName);

							licenseSelected = true;
						}
					}
				}
			}
		}
	}

	if (!licenseSelected){
		LicenseCombo->setCurrentIndex(-1);
	}
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CProductInstanceInfoEditorComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imtlic::IProductInstanceInfo* productInstanceInfoPtr = GetObservedObject();
	Q_ASSERT(productInstanceInfoPtr != nullptr);

	UpdateProductsCombo();
	UpdateLicensesCombo();

	ProductInstanceIdEdit->setText(productInstanceInfoPtr->GetProductInstanceId());
}


void CProductInstanceInfoEditorComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();

	imtlic::IProductInstanceInfo* productInstanceInfoPtr = GetObservedObject();
	Q_ASSERT(productInstanceInfoPtr != nullptr);

	m_productCollectionObserver.RegisterObject(productInstanceInfoPtr->GetProductDatabase(), &CProductInstanceInfoEditorComp::OnProductsUpdated);
}


void CProductInstanceInfoEditorComp::OnGuiModelDetached()
{
	m_productCollectionObserver.UnregisterAllObjects();

	BaseClass::OnGuiModelDetached();
}


void CProductInstanceInfoEditorComp::UpdateModel() const
{
	imtlic::IProductInstanceInfo* productInstanceInfoPtr = GetObservedObject();
	Q_ASSERT(productInstanceInfoPtr != nullptr);

	istd::CChangeGroup changeGroup(productInstanceInfoPtr);

	QByteArray currentProductId = ProductCombo->currentData().toByteArray();
	QByteArray customerId = CustomerCombo->currentData().toByteArray();
	QByteArray instanceId = ProductInstanceIdEdit->text().toUtf8();
	QByteArray licenseId = LicenseCombo->currentData().toByteArray();

	productInstanceInfoPtr->ResetData();
	productInstanceInfoPtr->SetupProductInstance(currentProductId, instanceId, customerId);

	if (!licenseId.isEmpty()){
		QDateTime validUntil;
		if (ExpireGroup->isChecked()){
			validUntil = ValidUntilDate->dateTime();
		}

		productInstanceInfoPtr->AddLicense(licenseId, validUntil);
	}
}


// reimplemented (iqtgui::CGuiComponentBase)

void CProductInstanceInfoEditorComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();
}


void CProductInstanceInfoEditorComp::OnGuiDestroyed()
{
	BaseClass::OnGuiDestroyed();
}


// private slots

void CProductInstanceInfoEditorComp::on_ProductInstanceIdEdit_editingFinished()
{
	DoUpdateModel();
}


void CProductInstanceInfoEditorComp::on_ProductCombo_currentIndexChanged(int /*index*/)
{
	DoUpdateModel();

	if (!IsUpdateBlocked()){
		UpdateBlocker updateBlocker(this);

		UpdateLicensesCombo();
	}
}


void CProductInstanceInfoEditorComp::on_LicenseCombo_currentIndexChanged(int /*index*/)
{
	DoUpdateModel();
}


void CProductInstanceInfoEditorComp::on_ValidUntilDate_dateTimeChanged(const QDateTime& /*dateTime*/)
{
	DoUpdateModel();
}


void CProductInstanceInfoEditorComp::on_ExpireGroup_toggled(bool /*toggled*/)
{
	DoUpdateModel();
}


} // namespace imtlicgui


