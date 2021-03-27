#include <imtauthgui/CContactInfoEditorComp.h>


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

void CContactInfoEditorComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imtauth::IContactInfo* contactPtr = GetObservedObject();
	Q_ASSERT(contactPtr != nullptr);

	EMailEdit->setText(contactPtr->GetMail());

	BirthdayEdit->setDate(contactPtr->GetBirthday());
	FirstNameEdit->setText(contactPtr->GetNameField(imtauth::IContactInfo::NFT_FIRST_NAME));
	LastNameEdit->setText(contactPtr->GetNameField(imtauth::IContactInfo::NFT_LAST_NAME));
	NicknameEdit->setText(contactPtr->GetNameField(imtauth::IContactInfo::NFT_NICKNAME));

	Addresses->clear();

	const imtauth::IAddressProvider* addressesPtr = contactPtr->GetAddresses();
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


void CContactInfoEditorComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();
}


void CContactInfoEditorComp::OnGuiModelDetached()
{
	BaseClass::OnGuiModelDetached();
}


void CContactInfoEditorComp::UpdateModel() const
{
	imtauth::IContactInfo* contactPtr = GetObservedObject();
	Q_ASSERT(contactPtr != nullptr);

	istd::CChangeGroup changeGroup(contactPtr);

	contactPtr->SetEmail(EMailEdit->text());
	contactPtr->SetBirthday(BirthdayEdit->date());
	contactPtr->SetNameField(imtauth::IContactInfo::NFT_FIRST_NAME, FirstNameEdit->text());
	contactPtr->SetNameField(imtauth::IContactInfo::NFT_LAST_NAME, LastNameEdit->text());
	contactPtr->SetNameField(imtauth::IContactInfo::NFT_NICKNAME, NicknameEdit->text());

	imtauth::IAddressManager* addressesPtr = dynamic_cast<imtauth::IAddressManager*>(
				const_cast<imtauth::IAddressProvider*>(contactPtr->GetAddresses()));

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

void CContactInfoEditorComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	AddAddress->setDefaultAction(&m_addAddressAction);
	RemoveAddress->setDefaultAction(&m_removeAddressAction);

	m_addAddressAction.setIcon(QIcon(":/Icons/Add"));
	m_removeAddressAction.setIcon(QIcon(":/Icons/Remove"));
}


void CContactInfoEditorComp::OnGuiDestroyed()
{
	BaseClass::OnGuiDestroyed();
}


// private slots

void CContactInfoEditorComp::on_EMailEdit_editingFinished()
{
	DoUpdateModel();
}


void CContactInfoEditorComp::on_BirthdayEdit_dateChanged(const QDate &date)
{
	DoUpdateModel();
}


void CContactInfoEditorComp::on_FirstNameEdit_editingFinished()
{
	DoUpdateModel();
}


void CContactInfoEditorComp::on_LastNameEdit_editingFinished()
{
	DoUpdateModel();
}


void CContactInfoEditorComp::on_NicknameEdit_editingFinished()
{
	DoUpdateModel();
}


void CContactInfoEditorComp::on_Addresses_itemSelectionChanged()
{
	RemoveAddress->setEnabled(Addresses->selectedItems().count() > 0);
}


void CContactInfoEditorComp::on_Addresses_itemChanged(QTreeWidgetItem *item, int column)
{
	DoUpdateModel();
}


void CContactInfoEditorComp::on_AddAddress_triggered(QAction *action)
{
	imtauth::IContactInfo* personPtr = GetObservedObject();
	Q_ASSERT(personPtr != nullptr);

	imtauth::IAddressManager* addressesPtr = dynamic_cast<imtauth::IAddressManager*>(
				const_cast<imtauth::IAddressProvider*>(personPtr->GetAddresses()));

	if (addressesPtr != nullptr){
		imtauth::CAddress address;

		addressesPtr->AddAddress(&address);
	}
}


void CContactInfoEditorComp::on_RemoveAddress_triggered(QAction *action)
{
	imtauth::IContactInfo* personPtr = GetObservedObject();
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


