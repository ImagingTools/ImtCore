#include <imtdbgui/CDatabaseLoginSettingsEditorComp.h>


// ACF includes
#include <istd/CChangeGroup.h>


namespace imtdbgui
{


// protected methods

// reimplemented (imod::IModelEditor)

void CDatabaseLoginSettingsEditorComp::UpdateModel() const
{
	imtdb::IDatabaseLoginSettings* objectPtr = GetObservedObject();
	Q_ASSERT(objectPtr != nullptr);

	istd::CChangeGroup changeGroup(objectPtr);

	objectPtr->SetHost(HostEdit->text());
	objectPtr->SetPort(PortEdit->text().toInt());
	objectPtr->SetDatabaseName(DatabaseNameEdit->text());
	objectPtr->SetUserName(UserEdit->text());
	objectPtr->SetPassword(PasswordEdit->text());
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CDatabaseLoginSettingsEditorComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imtdb::IDatabaseLoginSettings* objectPtr = GetObservedObject();
	Q_ASSERT(objectPtr != nullptr);

	HostEdit->setText(objectPtr->GetHost());
	PortEdit->setText(QString::number(objectPtr->GetPort()));
	DatabaseNameEdit->setText(objectPtr->GetDatabaseName());
	UserEdit->setText(objectPtr->GetUserName());
	PasswordEdit->setText(objectPtr->GetPassword());
}


// private slots

void CDatabaseLoginSettingsEditorComp::on_HostEdit_editingFinished()
{
	DoUpdateModel();
}


void CDatabaseLoginSettingsEditorComp::on_PortEdit_editingFinished()
{
	DoUpdateModel();
}


void CDatabaseLoginSettingsEditorComp::on_DatabaseNameEdit_editingFinished()
{
	DoUpdateModel();
}


void CDatabaseLoginSettingsEditorComp::on_UserEdit_editingFinished()
{
	DoUpdateModel();
}


void CDatabaseLoginSettingsEditorComp::on_PasswordEdit_editingFinished()
{
	DoUpdateModel();
}


} // namespace imtdbgui


