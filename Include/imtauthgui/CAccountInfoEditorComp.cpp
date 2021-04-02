#include <imtauthgui/CAccountInfoEditorComp.h>


// Qt includes
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>

// ACF includes
#include <ifilegui/CFileDialogLoaderComp.h>
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtauth/IAddressManager.h>
#include <imtauth/IContactInfo.h>
#include <imtauth/CAddress.h>


namespace imtauthgui
{


// public methods

CAccountInfoEditorComp::CAccountInfoEditorComp()
	:m_addressObserver(*this),
	m_blockComboChanged(false)
{
}


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CAccountInfoEditorComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imtauth::IAccountInfo* accountPtr = GetObservedObject();
	Q_ASSERT(accountPtr != nullptr);

	switch (accountPtr->GetAccountType()){
	case imtauth::IAccountInfo::AT_PERSON:
		AccountTypeCombo->setCurrentIndex(0);
		ContactStackedWidget->setCurrentIndex(0);
		break;

	case imtauth::IAccountInfo::AT_COMPANY:
		AccountTypeCombo->setCurrentIndex(1);
		ContactStackedWidget->setCurrentIndex(1);
		break;

	default:
		AccountTypeCombo->setCurrentIndex(-1);
		ContactStackedWidget->setCurrentIndex(-1);
	}

	AccountNameEdit->setText(accountPtr->GetAccountName());
	AccountDescriptionEdit->setText(accountPtr->GetAccountDescription());
	SetCompanyAddressVisibility(accountPtr->GetAccountType() == imtauth::IAccountInfo::AT_COMPANY);

	if (m_accountPictureObserverCompPtr.IsValid()){
		const iimg::IBitmap* bitmapPtr = &GetObservedObject()->GetAccountPicture();

		if (bitmapPtr != nullptr){
			if(!bitmapPtr->IsEmpty()){
				imod::IModel* accountPicureModelPtr = dynamic_cast<imod::IModel*>(
							const_cast<iimg::IBitmap*>(bitmapPtr));

				if (accountPicureModelPtr != nullptr){
					m_accountPictureModelProxy.SetModelPtr(accountPicureModelPtr);
					return;
				}
			}
		}

		m_accountPictureModelProxy.SetModelPtr(&m_emptyAccountPicture);
	}
}


void CAccountInfoEditorComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();

	imod::IModel* contactModelPtr = dynamic_cast<imod::IModel*>(
				const_cast<imtauth::IContactInfo*>(
							GetObservedObject()->GetAccountOwner()));

	if (contactModelPtr != nullptr){
		if (m_personContactEditorObserverCompPtr.IsValid()){
			contactModelPtr->AttachObserver(m_personContactEditorObserverCompPtr.GetPtr());
		}

		if (m_companyContactEditorObserverCompPtr.IsValid()){
			contactModelPtr->AttachObserver(m_companyContactEditorObserverCompPtr.GetPtr());
		}
	}
}


void CAccountInfoEditorComp::OnGuiModelDetached()
{
	imod::IModel* contactModelPtr = dynamic_cast<imod::IModel*>(
				const_cast<imtauth::IContactInfo*>(
							GetObservedObject()->GetAccountOwner()));

	if (contactModelPtr != nullptr){
		contactModelPtr->DetachAllObservers();
	}

	imod::IModel* accountPicureModelPtr = dynamic_cast<imod::IModel*>(
				const_cast<iimg::IBitmap*>(&GetObservedObject()->GetAccountPicture()));

	if (accountPicureModelPtr != nullptr){
		accountPicureModelPtr->DetachAllObservers();
	}

	BaseClass::OnGuiModelDetached();
}


void CAccountInfoEditorComp::UpdateModel() const
{
	imtauth::IAccountInfo* accountPtr = GetObservedObject();
	Q_ASSERT(accountPtr != nullptr);

	istd::CChangeGroup changeGroup(accountPtr);

	accountPtr->SetAccountType((imtauth::IAccountInfo::AccountType)AccountTypeCombo->currentIndex());
	accountPtr->SetAccountName(AccountNameEdit->text());
	accountPtr->SetAccountDescription(AccountDescriptionEdit->text());

	if (accountPtr->GetAccountType() == imtauth::IAccountInfo::AT_COMPANY){
		SetupCompanyAddress();
	}

	SetCompanyAddressVisibility(accountPtr->GetAccountType() == imtauth::IAccountInfo::AT_COMPANY);
}


// reimplemented (iqtgui::CGuiComponentBase)

void CAccountInfoEditorComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	m_blockComboChanged = true;
	AccountTypeCombo->addItem(tr("Personal"));
	AccountTypeCombo->addItem(tr("Company"));
	m_blockComboChanged = false;

	m_loadAccountPictureAction.setIcon(QIcon(":/Icons/Workflow"));
	m_removeAccountPictureAction.setIcon(QIcon(":/Icons/Remove"));
	m_loadAccountPictureAction.setText(tr("Load..."));
	m_removeAccountPictureAction.setText(tr("Remove"));

	LoadPicture->setDefaultAction(&m_loadAccountPictureAction);
	RemovePicture->setDefaultAction(&m_removeAccountPictureAction);

	if (m_accountPictureGuiCompPtr.IsValid() && m_accountPictureObserverCompPtr.IsValid()){
		m_accountPictureGuiCompPtr->CreateGui(AccountPicture);
	}

	if (m_personContactEditorCompPtr.IsValid() && m_personContactEditorObserverCompPtr.IsValid()){
		m_personContactEditorCompPtr->CreateGui(PersonContactEditor);
	}

	if (m_companyContactEditorCompPtr.IsValid() && m_companyContactEditorObserverCompPtr.IsValid()){
		m_companyContactEditorCompPtr->CreateGui(CompanyContactEditor);
	}
}


void CAccountInfoEditorComp::OnGuiDestroyed()
{
	m_addressObserver.UnregisterAllObjects();

	if (m_companyContactEditorCompPtr.IsValid() && m_companyContactEditorCompPtr->IsGuiCreated()){
		m_companyContactEditorCompPtr->DestroyGui();
	}

	if (m_personContactEditorCompPtr.IsValid() && m_personContactEditorCompPtr->IsGuiCreated()){
		m_personContactEditorCompPtr->DestroyGui();
	}

	if (m_accountPictureGuiCompPtr.IsValid() && m_accountPictureGuiCompPtr->IsGuiCreated()){
		m_accountPictureGuiCompPtr->DestroyGui();
	}

	BaseClass::OnGuiDestroyed();
}


// reimplemented (icomp::CComponentBase)

void CAccountInfoEditorComp::OnComponentCreated()
{
	QIcon icon = QIcon(":/Icons/Account"); 
	if (!icon.isNull()){
		iimg::CBitmap bitmap(icon.pixmap(300, 300).toImage());
		m_emptyAccountPicture.CopyFrom(bitmap);
	}

	if (m_accountPictureObserverCompPtr.IsValid()){
		m_accountPictureModelProxy.AttachObserver(m_accountPictureObserverCompPtr.GetPtr());
	}
}


void CAccountInfoEditorComp::OnComponentDestroyed()
{
	m_accountPictureModelProxy.DetachAllObservers();
}


// private methods

void CAccountInfoEditorComp::SetCompanyAddressVisibility(bool visibility) const
{
	if (visibility){
		imtauth::IAccountInfo* accountPtr = GetObservedObject();
		if (accountPtr != nullptr){
			const imtauth::IContactInfo* contactPtr = accountPtr->GetAccountOwner();
			if (contactPtr != nullptr){
				const imtauth::IAddressProvider* addressProviderPtr = contactPtr->GetAddresses();

				imtbase::ICollectionInfo::Ids ids = addressProviderPtr->GetAddressList().GetElementIds();
				Q_ASSERT(ids.count() == 1);

				m_addressObserver.RegisterObject(
							addressProviderPtr->GetAddress(ids[0]),
							&CAccountInfoEditorComp::OnAddressUpdated);
			}

			AddressGroup->show();
		}
	}
	else{
		m_addressObserver.UnregisterAllObjects();
		AddressGroup->hide();
	}
}


void CAccountInfoEditorComp::SetupCompanyAddress() const
{
	imtauth::IAccountInfo* accountPtr = GetObservedObject();
	if (accountPtr != nullptr){
		const imtauth::IContactInfo* contactPtr = accountPtr->GetAccountOwner();
		if (contactPtr != nullptr){
			imtauth::IAddressManager* addressManager = dynamic_cast<imtauth::IAddressManager*>(
						const_cast<imtauth::IAddressProvider*>(contactPtr->GetAddresses()));

			if (addressManager != nullptr){
				imtbase::ICollectionInfo::Ids ids = addressManager->GetAddressList().GetElementIds();
				if (ids.isEmpty()){
					imtauth::CAddress* addressPtr = new imtauth::CAddress();

					addressManager->AddAddress(addressPtr);
				}
				else if (ids.count() > 1){
					for (int i = 1; i < ids.count(); i++){
						addressManager->RemoveAddress(ids[i]);
					}
				}
			}
		}
	}
}


void CAccountInfoEditorComp::OnAddressUpdated(const istd::IChangeable::ChangeSet& /*changeSet*/, const imtauth::IAddress* addressPtr)
{
	CountryEdit->setText(addressPtr->GetCountry());
	CityEdit->setText(addressPtr->GetCity());
	PostalCodeEdit->setText(QString::number(addressPtr->GetPostalCode()));
	StreetEdit->setText(addressPtr->GetStreet());
}


imtauth::IAddress* CAccountInfoEditorComp::GetCompanyAddress()
{
	imtauth::IAccountInfo* accountPtr = GetObservedObject();
	if (accountPtr != nullptr){
		const imtauth::IContactInfo* contactPtr = accountPtr->GetAccountOwner();
		if (contactPtr != nullptr){
			const imtauth::IAddressProvider* addressProviderPtr = contactPtr->GetAddresses();

			if (addressProviderPtr != nullptr){
				imtbase::ICollectionInfo::Ids ids = addressProviderPtr->GetAddressList().GetElementIds();
				Q_ASSERT(ids.count() == 1);

				return const_cast<imtauth::IAddress*>(addressProviderPtr->GetAddress(ids[0]));
			}
		}
	}

	return nullptr;
}


// private slots

void CAccountInfoEditorComp::on_ContactCombo_currentIndexChanged(int index)
{
	DoUpdateModel();
}


void CAccountInfoEditorComp::on_AccountTypeCombo_currentIndexChanged(int /*index*/)
{
	if (m_blockComboChanged){
		return;
	}

	DoUpdateModel();

	imtauth::IAccountInfo* accountPtr = GetObservedObject();
	if (accountPtr != nullptr){
		switch (accountPtr->GetAccountType()){
		case imtauth::IAccountInfo::AT_PERSON:
			ContactStackedWidget->setCurrentIndex(0);
			break;
		case imtauth::IAccountInfo::AT_COMPANY:
			ContactStackedWidget->setCurrentIndex(1);
			break;
		default:
			ContactStackedWidget->setCurrentIndex(-1);
		}
	}
}


void CAccountInfoEditorComp::on_AccountNameEdit_editingFinished()
{
	DoUpdateModel();
}


void CAccountInfoEditorComp::on_AccountDescriptionEdit_editingFinished()
{
	DoUpdateModel();
}


void CAccountInfoEditorComp::on_CountryEdit_editingFinished()
{
	if (!m_isReadOnly && !IsUpdateBlocked() && IsModelAttached()){
		UpdateBlocker updateBlocker(this);

		imtauth::IAddress* addressPtr = GetCompanyAddress();
		if (addressPtr != nullptr){
			addressPtr->SetCountry(CountryEdit->text());
		}
	}
}


void CAccountInfoEditorComp::on_CityEdit_editingFinished()
{
	if (!m_isReadOnly && !IsUpdateBlocked() && IsModelAttached()){
		UpdateBlocker updateBlocker(this);

		imtauth::IAddress* addressPtr = GetCompanyAddress();
		if (addressPtr != nullptr){
			addressPtr->SetCity(CityEdit->text());
		}
	}
}


void CAccountInfoEditorComp::on_PostalCodeEdit_editingFinished()
{
	if (!m_isReadOnly && !IsUpdateBlocked() && IsModelAttached()){
		UpdateBlocker updateBlocker(this);

		imtauth::IAddress* addressPtr = GetCompanyAddress();
		if (addressPtr != nullptr){
			addressPtr->SetPostalCode(PostalCodeEdit->text().toInt());
		}
	}
}


void CAccountInfoEditorComp::on_StreetEdit_editingFinished()
{
	if (!m_isReadOnly && !IsUpdateBlocked() && IsModelAttached()){
		UpdateBlocker updateBlocker(this);

		imtauth::IAddress* addressPtr = GetCompanyAddress();
		if (addressPtr != nullptr){
			addressPtr->SetStreet(StreetEdit->text());
		}
	}
}


void CAccountInfoEditorComp::on_LoadPicture_triggered(QAction *action)
{
	imtauth::IAccountInfo* accountPtr = GetObservedObject();
	Q_ASSERT(accountPtr != nullptr);

	QStringList allExt;
	QStringList filterList;

	const ifile::IFilePersistence* filePersistencePtr = m_bitmapLoaderCompPtr.GetPtr();
	if (filePersistencePtr != nullptr){
		ifilegui::CFileDialogLoaderComp::AppendLoaderFilterList(*filePersistencePtr, nullptr, ifile::IFileTypeInfo::QF_LOAD, allExt, filterList, false);
	}

	if (filterList.size() > 1){
		filterList.prepend(tr("All known file types (%1)").arg("*." + allExt.join(" *.")));
	}

	if (filterList.size() == 0){
		filterList.prepend(tr("All file types (%1)").arg("*.*"));
	}

	QString representationFilePath = QFileDialog::getOpenFileName(GetWidget(), tr("Select picture file"), "", filterList.join("\n"));
	if (!representationFilePath.isEmpty()){
		QImage image(representationFilePath);
		if (!image.isNull()){
			image = image.scaled(300, 300);

			iimg::CBitmap bitmap(image);

			accountPtr->SetAccountPicture(bitmap);
		}
	}
}


void CAccountInfoEditorComp::on_RemovePicture_triggered(QAction *action)
{
	imtauth::IAccountInfo* accountPtr = GetObservedObject();
	Q_ASSERT(accountPtr != nullptr);

	iimg::CBitmap bitmap;

	accountPtr->SetAccountPicture(bitmap);
}


} // namespace imtauthgui


