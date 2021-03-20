#include <imtauthgui/CAddressEditorComp.h>


// ACF includes
#include <istd/CChangeGroup.h>


namespace imtauthgui
{


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CAddressEditorComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imtauth::IAddress* addressPtr = GetObservedObject();
	Q_ASSERT(addressPtr != nullptr);

	CountryEdit->setText(addressPtr->GetCountry());
	CityEdit->setText(addressPtr->GetCity());
	PostalCodeEdit->setText(QString("%1").arg(addressPtr->GetPostalCode()));
}


void CAddressEditorComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();
}


void CAddressEditorComp::OnGuiModelDetached()
{
	BaseClass::OnGuiModelDetached();
}


void CAddressEditorComp::UpdateModel() const
{
	imtauth::IAddress* addressPtr = GetObservedObject();
	Q_ASSERT(addressPtr != nullptr);

	istd::CChangeGroup changeGroup(addressPtr);

	addressPtr->SetCountry(CountryEdit->text());
	addressPtr->SetCity(CityEdit->text());
	addressPtr->SetPostalCode(PostalCodeEdit->text().toInt());
}


// reimplemented (iqtgui::CGuiComponentBase)

void CAddressEditorComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();
}


void CAddressEditorComp::OnGuiDestroyed()
{
	BaseClass::OnGuiDestroyed();
}


// private slots

void CAddressEditorComp::on_CountryEdit_editingFinished()
{
	DoUpdateModel();
}


void CAddressEditorComp::on_CityEdit_editingFinished()
{
	DoUpdateModel();
}


void CAddressEditorComp::on_PostalCodeEdit_editingFinished()
{
	DoUpdateModel();
}


} // namespace imtauthgui


