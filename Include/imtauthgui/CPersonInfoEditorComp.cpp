#include <imtauthgui/CPersonInfoEditorComp.h>


// ACF includes
#include <istd/CChangeGroup.h>


namespace imtauthgui
{


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CPersonInfoEditorComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imtauth::IAddress* addressPtr = GetObservedObject();
	Q_ASSERT(addressPtr != nullptr);

	CountryEdit->setText(addressPtr->GetCountry());
	CityEdit->setText(addressPtr->GetCity());
	PostalCodeEdit->setText(QString("%1").arg(addressPtr->GetPostalCode()));
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
	imtauth::IAddress* addressPtr = GetObservedObject();
	Q_ASSERT(addressPtr != nullptr);

	istd::CChangeGroup changeGroup(addressPtr);

	addressPtr->SetCountry(CountryEdit->text());
	addressPtr->SetCity(CityEdit->text());
	addressPtr->SetPostalCode(PostalCodeEdit->text().toInt());
}


// reimplemented (iqtgui::CGuiComponentBase)

void CPersonInfoEditorComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();
}


void CPersonInfoEditorComp::OnGuiDestroyed()
{
	BaseClass::OnGuiDestroyed();
}


// private slots

void CPersonInfoEditorComp::on_CountryEdit_editingFinished()
{
	DoUpdateModel();
}


void CPersonInfoEditorComp::on_CityEdit_editingFinished()
{
	DoUpdateModel();
}


void CPersonInfoEditorComp::on_PostalCodeEdit_editingFinished()
{
	DoUpdateModel();
}


} // namespace imtauthgui


