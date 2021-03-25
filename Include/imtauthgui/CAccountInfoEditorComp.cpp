#include <imtauthgui/CAccountInfoEditorComp.h>


// Qt includes
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>

// ACF includes
#include <ifilegui/CFileDialogLoaderComp.h>
#include <iimg/CBitmap.h>
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtauth/IContactInfo.h>


namespace imtauthgui
{


// public methods

CAccountInfoEditorComp::CAccountInfoEditorComp()
	:m_isComboChangedSignalBlocked(false)
{
}


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CAccountInfoEditorComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imtauth::IAccountInfo* accountPtr = GetObservedObject();
	Q_ASSERT(accountPtr != nullptr);

	m_isComboChangedSignalBlocked = true;

	switch (accountPtr->GetAccountType()){
	case imtauth::IAccountInfo::AT_PERSON:
		AccountTypeCombo->setCurrentIndex(0);
		break;
	case imtauth::IAccountInfo::AT_COMPANY:
		AccountTypeCombo->setCurrentIndex(1);
		break;
	default:
		AccountTypeCombo->setCurrentIndex(-1);
	}

	m_isComboChangedSignalBlocked = false;

	AccountNameEdit->setText(accountPtr->GetAccountName());
	AccountDescriptionEdit->setText(accountPtr->GetAccountDescription());
}


void CAccountInfoEditorComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();

	if (m_accountPictureObserverCompPtr.IsValid()){
		imod::IModel* accountPicureModelPtr = dynamic_cast<imod::IModel*>(
					const_cast<iimg::IBitmap*>(&GetObservedObject()->GetAccountPicture()));

		if (accountPicureModelPtr != nullptr){
			accountPicureModelPtr->AttachObserver(m_accountPictureObserverCompPtr.GetPtr());
		}
	}

	if (m_contactEditorObserverCompPtr.IsValid()){
		imod::IModel* contactModelPtr = dynamic_cast<imod::IModel*>(
					const_cast<imtauth::IContactInfo*>(
								GetObservedObject()->GetAccountOwner()));

		if (contactModelPtr != nullptr){
			contactModelPtr->AttachObserver(m_contactEditorObserverCompPtr.GetPtr());
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
}


// reimplemented (iqtgui::CGuiComponentBase)

void CAccountInfoEditorComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	m_isComboChangedSignalBlocked = true;
	AccountTypeCombo->addItem(tr("Personal"));
	AccountTypeCombo->addItem(tr("Company"));
	m_isComboChangedSignalBlocked = false;

	m_loadAccountPictureAction.setIcon(QIcon(":/Icons/Workflow"));
	m_removeAccountPictureAction.setIcon(QIcon(":/Icons/Remove"));
	m_loadAccountPictureAction.setText(tr("Load..."));
	m_removeAccountPictureAction.setText(tr("Remove"));

	LoadPicture->setDefaultAction(&m_loadAccountPictureAction);
	RemovePicture->setDefaultAction(&m_removeAccountPictureAction);

	if (m_accountPictureGuiCompPtr.IsValid() && m_accountPictureObserverCompPtr.IsValid()){
		m_accountPictureGuiCompPtr->CreateGui(AccountPicture);
	}

	if (m_contactEditorCompPtr.IsValid() && m_contactEditorObserverCompPtr.IsValid()){
		m_contactEditorCompPtr->CreateGui(ContactEditor);
	}
}


void CAccountInfoEditorComp::OnGuiDestroyed()
{
	if (m_contactEditorCompPtr.IsValid() && m_contactEditorCompPtr->IsGuiCreated()){
		m_contactEditorCompPtr->DestroyGui();
	}

	if (m_accountPictureGuiCompPtr.IsValid() && m_accountPictureGuiCompPtr->IsGuiCreated()){
		m_accountPictureGuiCompPtr->DestroyGui();
	}

	BaseClass::OnGuiDestroyed();
}


// private slots

void CAccountInfoEditorComp::on_ContactCombo_currentIndexChanged(int index)
{
	if (!m_isComboChangedSignalBlocked){
		DoUpdateModel();
	}
}


void CAccountInfoEditorComp::on_AccountTypeCombo_currentIndexChanged(int index)
{
	if (!m_isComboChangedSignalBlocked){
		DoUpdateModel();
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


