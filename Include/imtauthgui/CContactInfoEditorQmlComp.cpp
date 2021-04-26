#include <imtauthgui/CContactInfoEditorQmlComp.h>


// Qt includes
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>

// ACF includes
#include <istd/CChangeGroup.h>
#include <iser/CJsonStringWriteArchive.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtauth/IAddressManager.h>
#include <imtauth/CAddress.h>


namespace imtauthgui
{


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CContactInfoEditorQmlComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imtauth::IContactInfo* contactPtr = GetObservedObject();
	Q_ASSERT(contactPtr != nullptr);

}


void CContactInfoEditorQmlComp::OnGuiModelAttached()
{
//	BaseClass::OnGuiModelAttached();
}


void CContactInfoEditorQmlComp::OnGuiModelDetached()
{
//	BaseClass::OnGuiModelDetached();
}


void CContactInfoEditorQmlComp::UpdateModel() const
{
	imtauth::IContactInfo* contactPtr = GetObservedObject();
	Q_ASSERT(contactPtr != nullptr);

	istd::CChangeGroup changeGroup(contactPtr);


}


// reimplemented (iqtgui::CGuiComponentBase)

void CContactInfoEditorQmlComp::OnGuiCreated()
{
//	BaseClass::OnGuiCreated();

}


void CContactInfoEditorQmlComp::OnGuiDestroyed()
{
//	BaseClass::OnGuiDestroyed();
}


// private slots

void CContactInfoEditorQmlComp::on_EMailEdit_editingFinished()
{
//	DoUpdateModel();
}


void CContactInfoEditorQmlComp::on_BirthdayEdit_dateChanged(const QDate &date)
{
//	DoUpdateModel();
}


void CContactInfoEditorQmlComp::on_GenderCombo_currentIndexChanged(int index)
{
//	DoUpdateModel();
}


void CContactInfoEditorQmlComp::on_FirstNameEdit_editingFinished()
{
//	DoUpdateModel();
}


void CContactInfoEditorQmlComp::on_LastNameEdit_editingFinished()
{
//	DoUpdateModel();
}


void CContactInfoEditorQmlComp::on_NicknameEdit_editingFinished()
{
//	DoUpdateModel();
}


void CContactInfoEditorQmlComp::on_Addresses_itemSelectionChanged()
{
//	RemoveAddress->setEnabled(Addresses->selectedItems().count() > 0);
}



void CContactInfoEditorQmlComp::on_AddAddress_triggered(QAction *action)
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


void CContactInfoEditorQmlComp::on_RemoveAddress_triggered(QAction *action)
{
	imtauth::IContactInfo* personPtr = GetObservedObject();
	Q_ASSERT(personPtr != nullptr);

	imtauth::IAddressManager* addressesPtr = dynamic_cast<imtauth::IAddressManager*>(
		const_cast<imtauth::IAddressProvider*>(personPtr->GetAddresses()));

	if (addressesPtr != nullptr){
		istd::CChangeGroup changeGroup(addressesPtr);

//		QList<QTreeWidgetItem*> items = Addresses->selectedItems();
//		for (const QTreeWidgetItem* item : items){
//			addressesPtr->RemoveAddress(item->data(0, Qt::UserRole).toByteArray());
//		}
	}
}


// reimplemented (iqtgui::IGuiObject)

bool CContactInfoEditorQmlComp::IsGuiCreated() const
{
	if(m_guiCompPtr.IsValid())
		return m_guiCompPtr->IsGuiCreated();
	return false;
}


bool CContactInfoEditorQmlComp::CreateGui(QWidget *parentPtr)
{
	if(m_guiCompPtr.IsValid())
		return m_guiCompPtr->CreateGui(parentPtr);
	return false;
}


bool CContactInfoEditorQmlComp::DestroyGui()
{
	if(m_guiCompPtr.IsValid())
		return m_guiCompPtr->DestroyGui();
	return false;
}


QWidget *CContactInfoEditorQmlComp::GetWidget() const
{
	if(m_guiCompPtr.IsValid())
		return m_guiCompPtr->GetWidget();
	return nullptr;
}


void CContactInfoEditorQmlComp::OnTryClose(bool *ignoredPtr)
{
	if(m_guiCompPtr.IsValid())
		m_guiCompPtr->OnTryClose(ignoredPtr);
}


bool CContactInfoEditorQmlComp::CreateItem(QQmlEngine *enginePtr)
{
	imtauth::IContactInfo* contactPtr = GetObservedObject();
	Q_ASSERT(contactPtr != nullptr);

	m_treeItemModel.setData("firstName", contactPtr->GetNameField(imtauth::IContactInfo::NFT_FIRST_NAME));
	m_treeItemModel.setData("lastName", contactPtr->GetNameField(imtauth::IContactInfo::NFT_LAST_NAME));
	m_treeItemModel.setData("nicName", contactPtr->GetNameField(imtauth::IContactInfo::NFT_NICKNAME));
	qmlRegisterType<CTreeItemModel>("ACF", 1, 0, "TreeItemModel");

	const imtauth::IAddressProvider* addressesPtr = contactPtr->GetAddresses();
	if (addressesPtr != nullptr){
		imtbase::ICollectionInfo::Ids ids = addressesPtr->GetAddressList().GetElementIds();
		m_treeItemModel.addTreeModel("adresses");
		CTreeItemModel *addressesModel = m_treeItemModel.getTreeItemModel("adresses");
		for (const QByteArray& id : ids){
			const imtauth::IAddress* addressPtr = addressesPtr->GetAddress(id);
			if (addressPtr != nullptr){
				int index = addressesModel->addItem();
				addressesModel->setData("country",addressPtr->GetCountry(), index);
				addressesModel->setData("city",addressPtr->GetCity(), index);
				addressesModel->setData("postalCode",addressPtr->GetPostalCode(), index);
				addressesModel->setData("street",addressPtr->GetStreet(), index);
			}
		}
	}

	QByteArray jsonContactInfo;
	GetJsonContactInfo(jsonContactInfo);

	enginePtr->addImportPath("qrc:/qml");

	QFile file("TextFieldCustom.qml");
	if(file.open(QIODevice::ReadOnly)){
		QByteArray ba = file.readAll();
		qDebug() << ba;
	}

	QQmlContext *ctxt = enginePtr->rootContext();
	ctxt->setContextProperty("contactInfoModel", &m_treeItemModel);

	return BaseClass::CreateItem(enginePtr,{{"contactInfoModel", QVariant::fromValue(&m_treeItemModel)}});
}


bool CContactInfoEditorQmlComp::GetJsonContactInfo(QByteArray &jsonContactInfo)
{
	iser::CJsonStringWriteArchive archive(jsonContactInfo);

	bool retVal = m_treeItemModel.Serialize(archive);
	return retVal;
}


} // namespace imtauthgui


