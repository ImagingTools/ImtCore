#include <imtlicgui/CProductInstanceInfoViewComp.h>


// Qt includes
#include <QtCore/QUuid>
#include <QtCore/QStandardPaths>
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

// ACF includes
#include <istd/CSystem.h>
#include <ilog/CMessageContainer.h>
#include <iprm/CNameParam.h>
#include <ifilegui/CFileDialogLoaderComp.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtlic/ILicenseInstance.h>


namespace imtlicgui
{


// public methods

CProductInstanceInfoViewComp::CProductInstanceInfoViewComp()
	:m_importLicenseEnablerObserver(*this),
	m_licenseRequestEnablerObserver(*this),
	m_licenseStatusObserver(*this)
{
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CProductInstanceInfoViewComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imtlic::IProductInstanceInfo* productInstanceInfoPtr = GetObservedObject();
	Q_ASSERT(productInstanceInfoPtr != nullptr);

	UpdateProductName();
	UpdateFeatureTree();

	ProductInstanceIdEdit->setText(productInstanceInfoPtr->GetProductInstanceId());
	CustomerNameEdit->setText(productInstanceInfoPtr->GetCustomerId());
}


// reimplemented (iqtgui::CGuiComponentBase)

void CProductInstanceInfoViewComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	LicenseManagementGroup->setVisible(*m_showLicenseControlPanelAttrPtr);

	NewLicenseRequestButton->setVisible(*m_showLicenseRequestAttrPtr);
	LicenseRequestDescriptionLabel->setVisible(*m_showLicenseRequestAttrPtr);

	FeatureTree->header()->setSectionResizeMode(QHeaderView::Stretch);

	m_importLicenseEnablerObserver.RegisterObject(m_importLicenseEnablerCompPtr.GetPtr(), &CProductInstanceInfoViewComp::OnImportLicenseEnabled);
	m_licenseRequestEnablerObserver.RegisterObject(m_licenseRequestEnablerCompPtr.GetPtr(), &CProductInstanceInfoViewComp::OnLicenseRequestEnabled);
	m_licenseStatusObserver.RegisterObject(m_licenseStatusCompPtr.GetPtr(), &CProductInstanceInfoViewComp::OnLicenseStatusChanged);
}


void CProductInstanceInfoViewComp::OnGuiDestroyed()
{
	m_importLicenseEnablerObserver.UnregisterAllObjects();
	m_licenseRequestEnablerObserver.UnregisterAllObjects();
	m_licenseStatusObserver.UnregisterAllObjects();

	BaseClass::OnGuiDestroyed();
}


void CProductInstanceInfoViewComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();

	LicenseRequestDescriptionLabel->setText(*m_licenseRequestDescriptionTextAttrPtr);

	imtlic::ILicenseStatus* licenseStatusPtr = m_licenseStatusObserver.GetObjectAt<imtlic::ILicenseStatus>(0);
	if (licenseStatusPtr != nullptr){
		OnLicenseStatusChanged(istd::IChangeable::GetAnyChange(), licenseStatusPtr);
	}
}


// private methods

void CProductInstanceInfoViewComp::UpdateProductName()
{
	ProductNameEdit->clear();

	imtlic::IProductInstanceInfo* productInstanceInfoPtr = GetObservedObject();
	Q_ASSERT(productInstanceInfoPtr != nullptr);

	QString selectedProductId = productInstanceInfoPtr->GetProductId();
	ProductNameEdit->setText(qPrintable(selectedProductId));

	const imtbase::IObjectCollection* productsCollectionPtr = productInstanceInfoPtr->GetProductDatabase();
	if (productsCollectionPtr != nullptr){
		const imtbase::IObjectCollectionInfo::Ids productIds = productsCollectionPtr->GetElementIds();
		for (const QByteArray& productId : productIds){
			QString productName = productsCollectionPtr->GetElementInfo(productId, imtbase::ICollectionInfo::EIT_NAME).toString();

			ProductNameEdit->setText(productName);
		}
	}
}


void CProductInstanceInfoViewComp::UpdateFeatureTree()
{
	FeatureTree->clear();

	imtlic::IProductInstanceInfo* productInstanceInfoPtr = GetObservedObject();
	Q_ASSERT(productInstanceInfoPtr != nullptr);

	const imtbase::ICollectionInfo& licenseList = productInstanceInfoPtr->GetLicenseInstances();

	const imtbase::IObjectCollectionInfo::Ids allInstanceLicenseIds = licenseList.GetElementIds();
	for (const QByteArray& licenseCollectionId : allInstanceLicenseIds){
		const imtlic::ILicenseInstance* licenseInstancePtr = productInstanceInfoPtr->GetLicenseInstance(licenseCollectionId);
		if (licenseInstancePtr != nullptr){
			QString licenseName = licenseInstancePtr->GetLicenseName();
			QByteArray licenseId = licenseInstancePtr->GetLicenseId();

			licenseName += " (" + licenseId + ")";
			QString licenseNameText = QString(licenseName);

			QString licenseExpirationText;

			QDate date = licenseInstancePtr->GetExpiration().date();
			if (date.isValid()){
				QLocale locale;
				licenseExpirationText = locale.toString(licenseInstancePtr->GetExpiration().date(), QLocale::ShortFormat);
			}
			else{
				licenseExpirationText = tr("Unlimited");
			}

//			imtlic::ILicenseInstance::FeatureInfos features = licenseInstancePtr->GetFeatureInfos();
//			for (const imtlic::ILicenseInstance::FeatureInfo& feature : features){
//				QTreeWidgetItem* featureItemPtr = new QTreeWidgetItem();

//				featureItemPtr->setText(0, feature.name + " (" + feature.id + ")");
//				featureItemPtr->setText(0, feature.name + " (" + feature.id + ")");
//				featureItemPtr->setText(1, licenseNameText);
//				featureItemPtr->setText(2, licenseExpirationText);

//				FeatureTree->addTopLevelItem(featureItemPtr);
//			}
		}
	}
}


void CProductInstanceInfoViewComp::OnImportLicenseEnabled(
			const istd::IChangeable::ChangeSet& /*changeSet*/,
			const iprm::IEnableableParam* licenseImportEnablerPtr)
{
	Q_ASSERT(licenseImportEnablerPtr != nullptr);

	LoadLicenseButton->setEnabled(licenseImportEnablerPtr->IsEnabled() && m_licenseControllerCompPtr.IsValid());
}


void CProductInstanceInfoViewComp::OnLicenseRequestEnabled(
			const istd::IChangeable::ChangeSet& /*changeSet*/,
			const iprm::IEnableableParam* licenseRequstEnablerPtr)
{
	Q_ASSERT(licenseRequstEnablerPtr != nullptr);

	NewLicenseRequestButton->setEnabled(licenseRequstEnablerPtr->IsEnabled());
}


void CProductInstanceInfoViewComp::OnLicenseStatusChanged(const istd::IChangeable::ChangeSet& /*changeSet*/, const imtlic::ILicenseStatus* licenseStatusPtr)
{
	Q_ASSERT(licenseStatusPtr != nullptr);

	LicensePathLabel->setText(licenseStatusPtr->GetLicenseLocation());

	QString statusStyle = ((licenseStatusPtr->GetLicenseStatusFlags() & imtlic::ILicenseStatus::LSF_LICENSE_VALID) == 0) ? "color: red;": "";

	LicensePathLabel->setStyleSheet(statusStyle);
}


// private slots

void CProductInstanceInfoViewComp::on_NewLicenseRequestButton_clicked()
{
	if (m_licenseKeyPersistenceCompPtr.IsValid() && m_encryptionKeysProviderCompPtr.IsValid()){
		QStringList allExt;
		QStringList filterList;
		ifilegui::CFileDialogLoaderComp::AppendLoaderFilterList(*m_licenseKeyPersistenceCompPtr, nullptr, ifile::IFileTypeInfo::QF_SAVE, allExt, filterList, false);

		if (filterList.size() > 1){
			filterList.prepend(tr("All known file types (%1)").arg("*." + allExt.join(" *.")));
		}

		if (filterList.size() == 0){
			filterList.prepend(tr("All file types (%1)").arg("*.*"));
		}

		QString defaultFileName = QString("Transaction_Code_%1").arg(QDateTime::currentDateTime().toString("dd_MM_yyyy hh_mm_ss"));

		QString defaultExportPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
		
		defaultExportPath += "/" + defaultFileName;

		QString licenseKeyFilePath = QFileDialog::getSaveFileName(GetWidget(), tr("Create Transaction Code"), defaultExportPath, filterList.join("\n"));
		if (!licenseKeyFilePath.isEmpty()){
			iprm::CNameParam licenseKey;

			licenseKey.SetName(m_encryptionKeysProviderCompPtr->GetEncryptionKey(imtcrypt::IEncryptionKeysProvider::KT_PASSWORD));

			int state = m_licenseKeyPersistenceCompPtr->SaveToFile(licenseKey, licenseKeyFilePath);
			if (state == ifile::IFilePersistence::OS_FAILED){
				QMessageBox::critical(GetWidget(), tr("License Request Manager"), tr("License request file could not be saved"));
			}
		}
	}
}


void CProductInstanceInfoViewComp::on_LoadLicenseButton_clicked()
{
	if (m_licenseControllerCompPtr.IsValid()){
		QString licenseFilePath = QFileDialog::getOpenFileName(GetWidget(), tr("Select license file to be imported"), "", tr("License files (*.lic)"));
		if (!licenseFilePath.isEmpty()){
			ilog::CMessageContainer log;
			if (m_licenseControllerCompPtr->ImportLicense(licenseFilePath, &log)){
				QMessageBox::information(GetWidget(), tr("License Manager"), tr("License file successfully imported"));
			}
			else{
				ilog::CMessageContainer::Messages messages = log.GetMessages();

				if (!messages.empty()){
					QMessageBox::critical(GetWidget(), tr("License Manager"), messages.front()->GetInformationDescription());
				}
				else{
					QMessageBox::critical(GetWidget(), tr("License Manager"), tr("License could not be imported"));
				}
			}
		}
	}
}


} // namespace imtlicgui


