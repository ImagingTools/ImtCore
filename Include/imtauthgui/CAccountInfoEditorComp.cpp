#include <imtauthgui/CAccountInfoEditorComp.h>


// ACF includes
#include <istd/CChangeGroup.h>


namespace imtauthgui
{


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CAccountInfoEditorComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imtauth::IAddress* addressPtr = GetObservedObject();
	Q_ASSERT(addressPtr != nullptr);

	CountryEdit->setText(addressPtr->GetCountry());
	CityEdit->setText(addressPtr->GetCity());
	PostalCodeEdit->setText(QString("%1").arg(addressPtr->GetPostalCode()));
}


void CAccountInfoEditorComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();
}


void CAccountInfoEditorComp::OnGuiModelDetached()
{
	BaseClass::OnGuiModelDetached();
}


void CAccountInfoEditorComp::UpdateModel() const
{
	imtauth::IAddress* addressPtr = GetObservedObject();
	Q_ASSERT(addressPtr != nullptr);

	istd::CChangeGroup changeGroup(addressPtr);

	addressPtr->SetCountry(CountryEdit->text());
	addressPtr->SetCity(CityEdit->text());
	addressPtr->SetPostalCode(PostalCodeEdit->text().toInt());
}


// reimplemented (iqtgui::CGuiComponentBase)

void CAccountInfoEditorComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();
}


void CAccountInfoEditorComp::OnGuiDestroyed()
{
	BaseClass::OnGuiDestroyed();
}


// private slots

void CAccountInfoEditorComp::on_CountryEdit_editingFinished()
{
	DoUpdateModel();
}


void CAccountInfoEditorComp::on_CityEdit_editingFinished()
{
	DoUpdateModel();
}


void CAccountInfoEditorComp::on_PostalCodeEdit_editingFinished()
{
	DoUpdateModel();
}


} // namespace imtauthgui


