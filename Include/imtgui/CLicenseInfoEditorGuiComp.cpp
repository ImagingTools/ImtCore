#include <imtgui/CLicenseInfoEditorGuiComp.h>


// Acf includes
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtbase/CLicenseInfo.h>


namespace imtgui
{


// public methods

CLicenseInfoEditorGuiComp::CLicenseInfoEditorGuiComp()
{
}


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CLicenseInfoEditorGuiComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imtbase::ILicenseInfo* licenseInfoPtr = GetObservedObject();
	Q_ASSERT(licenseInfoPtr != nullptr);

	NameEdit->setText(licenseInfoPtr->GetLicenseName());
	IdEdit->setText(licenseInfoPtr->GetLicenseId());
	// TODO: package id combo
	ExpiredDate->setDateTime(licenseInfoPtr->GetExpiration());
}


void CLicenseInfoEditorGuiComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();
}


void CLicenseInfoEditorGuiComp::OnGuiModelDetached()
{
	BaseClass::OnGuiModelDetached();
}


void CLicenseInfoEditorGuiComp::UpdateModel() const
{
	imtbase::ILicenseInfo* licenseInfoPtr = GetObservedObject();
	Q_ASSERT(licenseInfoPtr != nullptr);

	imtbase::CLicenseInfo licenseInfo;

	licenseInfo.SetLicenseName(NameEdit->text());
	licenseInfo.SetLicenseId(IdEdit->text().toUtf8());
	// TODO: package id combo
	licenseInfo.SetExpiration(QDateTime(ExpiredDate->date(), QTime(0,0)));

	licenseInfoPtr->CopyFrom(licenseInfo);
}


// private slots

void CLicenseInfoEditorGuiComp::on_NameEdit_editingFinished()
{
	DoUpdateModel();
}


void CLicenseInfoEditorGuiComp::on_IdEdit_editingFinished()
{
	DoUpdateModel();
}


void CLicenseInfoEditorGuiComp::on_PackageCombo_currentTextChanged(const QString &text)
{
	DoUpdateModel();
}


void CLicenseInfoEditorGuiComp::on_ExpiredDate_dateTimeChanged(const QDateTime &datetime)
{
	DoUpdateModel();
}


} // namespace imtgui


