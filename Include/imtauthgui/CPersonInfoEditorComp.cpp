#include <imtauthgui/CPersonInfoEditorComp.h>


// ACF includes
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtauth/IAddressManager.h>
#include <imtauth/CAddress.h>


namespace imtauthgui
{


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CPersonInfoEditorComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imtauth::IPersonInfo* personPtr = GetObservedObject();
	Q_ASSERT(personPtr != nullptr);

	GenderCombo->addItem(tr("Diverse"));
	GenderCombo->addItem(tr("Female"));
	GenderCombo->addItem(tr("Male"));
	switch (personPtr->GetGenderType()){
	case imtauth::IPersonInfo::GT_FEMALE:
		GenderCombo->setCurrentIndex(1);
		break;
	case imtauth::IPersonInfo::GT_MALE:
		GenderCombo->setCurrentIndex(2);
		break;
	default:
		GenderCombo->setCurrentIndex(0);
	}

	BirthdayEdit->setDate(personPtr->GetBirthday());
	FirstNameEdit->setText(personPtr->GetNameField(imtauth::IPersonInfo::NFT_FIRST_NAME));
	LastNameEdit->setText(personPtr->GetNameField(imtauth::IPersonInfo::NFT_LAST_NAME));
	NicknameEdit->setText(personPtr->GetNameField(imtauth::IPersonInfo::NFT_NICKNAME));

	Addresses->clear();

	const imtauth::IAddressProvider* addressesPtr = personPtr->GetAddresses();
	if (addressesPtr != nullptr){
		imtbase::ICollectionInfo::Ids ids = addressesPtr->GetAddressList().GetElementIds();
		for (const QByteArray& id : ids){
			const imtauth::IAddress* addressPtr = addressesPtr->GetAddress(id);
			if (addressPtr != nullptr){
				QTreeWidgetItem* itemPtr = new QTreeWidgetItem({
							addressPtr->GetCountry(),
							addressPtr->GetCity(),
							QString::number(addressPtr->GetPostalCode())});

				itemPtr->setFlags(itemPtr->flags() | Qt::ItemIsEditable);
				itemPtr->setData(0, Qt::UserRole, id);
				Addresses->addTopLevelItem(itemPtr);
			}
		}
	}

	RemoveAddress->setEnabled(Addresses->selectedItems().count() > 0);
}


void CPersonInfoEditorComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();
}


void CPersonInfoEditorComp::OnGuiModelDetached()
{
	BaseClass::OnGuiModelDetached();
}


void CPersonInfoEditorComp::UpdateModel() const
{
	imtauth::IPersonInfo* personPtr = GetObservedObject();
	Q_ASSERT(personPtr != nullptr);

	istd::CChangeGroup changeGroup(personPtr);

	personPtr->SetGenderType(imtauth::IPersonInfo::GenderType(GenderCombo->currentIndex()));
	personPtr->SetBirthday(BirthdayEdit->date());
	personPtr->SetNameField(imtauth::IPersonInfo::NFT_FIRST_NAME, FirstNameEdit->text());
	personPtr->SetNameField(imtauth::IPersonInfo::NFT_LAST_NAME, LastNameEdit->text());
	personPtr->SetNameField(imtauth::IPersonInfo::NFT_NICKNAME, NicknameEdit->text());

	imtauth::IAddressManager* addressesPtr = dynamic_cast<imtauth::IAddressManager*>(
				const_cast<imtauth::IAddressProvider*>(personPtr->GetAddresses()));

	if (addressesPtr != nullptr){
		istd::CChangeGroup changeGroup(addressesPtr);

		addressesPtr->RemoveAllAddresses();

		int count = Addresses->topLevelItemCount();
		for (int i = 0; i < count; i++){
			QTreeWidgetItem* itemPtr = Addresses->topLevelItem(i);
			
			imtauth::CAddress address;
			address.SetCountry(itemPtr->text(0));
			address.SetCity(itemPtr->text(1));
			address.SetPostalCode(itemPtr->text(2).toInt());

			addressesPtr->AddAddress(&address);
		}
	}

}


// reimplemented (iqtgui::CGuiComponentBase)

void CPersonInfoEditorComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	AddAddress->setDefaultAction(&m_addAddressAction);
	RemoveAddress->setDefaultAction(&m_removeAddressAction);

	m_addAddressAction.setIcon(QIcon(":/Icons/Add"));
	m_removeAddressAction.setIcon(QIcon(":/Icons/Remove"));
}


void CPersonInfoEditorComp::OnGuiDestroyed()
{
	BaseClass::OnGuiDestroyed();
}


// private slots

void CPersonInfoEditorComp::on_GenderCombo_currentIndexChanged(int index)
{
	DoUpdateModel();
}


void CPersonInfoEditorComp::on_BirthdayEdit_dateChanged(const QDate &date)
{
	DoUpdateModel();
}


void CPersonInfoEditorComp::on_FirstNameEdit_editingFinished()
{
	DoUpdateModel();
}


void CPersonInfoEditorComp::on_LastNameEdit_editingFinished()
{
	DoUpdateModel();
}


void CPersonInfoEditorComp::on_NicknameEdit_editingFinished()
{
	DoUpdateModel();
}


void CPersonInfoEditorComp::on_Addresses_itemSelectionChanged()
{
	RemoveAddress->setEnabled(Addresses->selectedItems().count() > 0);
}


void CPersonInfoEditorComp::on_Addresses_itemChanged(QTreeWidgetItem *item, int column)
{
	DoUpdateModel();
}


void CPersonInfoEditorComp::on_AddAddress_triggered(QAction *action)
{
	imtauth::IPersonInfo* personPtr = GetObservedObject();
	Q_ASSERT(personPtr != nullptr);

	imtauth::IAddressManager* addressesPtr = dynamic_cast<imtauth::IAddressManager*>(
				const_cast<imtauth::IAddressProvider*>(personPtr->GetAddresses()));

	if (addressesPtr != nullptr){
		imtauth::CAddress address;

		addressesPtr->AddAddress(&address);
	}
}


void CPersonInfoEditorComp::on_RemoveAddress_triggered(QAction *action)
{
	imtauth::IPersonInfo* personPtr = GetObservedObject();
	Q_ASSERT(personPtr != nullptr);

	imtauth::IAddressManager* addressesPtr = dynamic_cast<imtauth::IAddressManager*>(
		const_cast<imtauth::IAddressProvider*>(personPtr->GetAddresses()));

	if (addressesPtr != nullptr){
		istd::CChangeGroup changeGroup(addressesPtr);

		QList<QTreeWidgetItem*> items = Addresses->selectedItems();
		for (const QTreeWidgetItem* item : items){
			addressesPtr->RemoveAddress(item->data(0, Qt::UserRole).toByteArray());
		}
	}
}


} // namespace imtauthgui


