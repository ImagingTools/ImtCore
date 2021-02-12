#include <imtlicgui/CLicenseInfoEditorGuiComp.h>


// Acf includes
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtlic/CLicenseInfo.h>


namespace imtlicgui
{


// public methods

CLicenseInfoEditorGuiComp::CLicenseInfoEditorGuiComp()
{
}


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CLicenseInfoEditorGuiComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imtlic::ILicenseInfo* licenseInfoPtr = GetObservedObject();
	Q_ASSERT(licenseInfoPtr != nullptr);

	NameEdit->setText(licenseInfoPtr->GetLicenseName());
	IdEdit->setText(licenseInfoPtr->GetLicenseId());
	// TODO: package id combo

	if (licenseInfoPtr->GetExpiration().isValid()){
		ExpireGroup->setChecked(true);
		ExpiredDate->setDateTime(licenseInfoPtr->GetExpiration());
	}
	else{
		ExpireGroup->setChecked(false);
	}
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
	imtlic::ILicenseInfo* licenseInfoPtr = GetObservedObject();
	Q_ASSERT(licenseInfoPtr != nullptr);

	imtlic::CLicenseInfo licenseInfo;

	licenseInfo.SetLicenseName(NameEdit->text());
	licenseInfo.SetLicenseId(IdEdit->text().toUtf8());
	// TODO: package id combo
	if (ExpireGroup->isChecked()){
		licenseInfo.SetExpiration(QDateTime(ExpiredDate->date(), QTime(0,0)));
	}
	else{
		licenseInfo.SetExpiration(QDateTime());
	}

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


void CLicenseInfoEditorGuiComp::on_PackageCombo_currentTextChanged(const QString& /*text*/)
{
	DoUpdateModel();
}


void CLicenseInfoEditorGuiComp::on_ExpiredDate_dateTimeChanged(const QDateTime& /*datetime*/)
{
	DoUpdateModel();
}


void CLicenseInfoEditorGuiComp::on_ExpireGroup_toggled(bool /*on*/)
{
	DoUpdateModel();
}


} // namespace imtlicgui


