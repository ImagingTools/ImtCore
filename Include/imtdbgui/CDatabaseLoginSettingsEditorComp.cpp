#include <imtdbgui/CDatabaseLoginSettingsEditorComp.h>


// ACF includes
#include <istd/CChangeGroup.h>


namespace imtdbgui
{


// public methods

CDatabaseLoginSettingsEditorComp::CDatabaseLoginSettingsEditorComp()
	:m_addressEditEnablerObserver(*this),
	m_databaseNameEditEnablerObserver(*this),
	m_generalEditEnablerObserver(*this)
{
}


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


// reimplemented (iqtgui::CGuiComponentBase)

void CDatabaseLoginSettingsEditorComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	if (m_addressEditEnablerCompPtr.IsValid()){
		m_addressEditEnablerObserver.RegisterObject(m_addressEditEnablerCompPtr.GetPtr(), &CDatabaseLoginSettingsEditorComp::OnAddressEditEnabled);

		HostEdit->setEnabled(m_addressEditEnablerCompPtr->IsEnabled());
		PortEdit->setEnabled(m_addressEditEnablerCompPtr->IsEnabled());
	}

	if (m_databaseNameEditEnablerCompPtr.IsValid()){
		m_databaseNameEditEnablerObserver.RegisterObject(m_databaseNameEditEnablerCompPtr.GetPtr(), &CDatabaseLoginSettingsEditorComp::OnDatabaseNameEditEnabled);

		DatabaseNameEdit->setEnabled(m_databaseNameEditEnablerCompPtr->IsEnabled());
	}

	if (m_generalEditEnablerCompPtr.IsValid()){
		m_generalEditEnablerObserver.RegisterObject(m_generalEditEnablerCompPtr.GetPtr(), &CDatabaseLoginSettingsEditorComp::OnGeneralEditEnabled);

		SetReadOnly(!m_generalEditEnablerCompPtr->IsEnabled());
	}

	TestConnectionFrame->setVisible(m_databaseConnectionCheckerCompPtr.IsValid());
}


void CDatabaseLoginSettingsEditorComp::OnGuiDestroyed()
{
	BaseClass::OnGuiDestroyed();

	m_addressEditEnablerObserver.UnregisterAllObjects();
	m_databaseNameEditEnablerObserver.UnregisterAllObjects();
	m_generalEditEnablerObserver.UnregisterAllObjects();
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


void CDatabaseLoginSettingsEditorComp::on_TestConnectionButton_clicked()
{
	Q_ASSERT(m_databaseConnectionCheckerCompPtr.IsValid());

	QString errorMessage;
	bool connectionSuccessful = m_databaseConnectionCheckerCompPtr->CheckDatabaseConnection(errorMessage);
	if (connectionSuccessful){
		errorMessage = tr("Database connection test was successful");
	}

	ConnectionTestResultLabel->setText(errorMessage);
}


// private methods

void CDatabaseLoginSettingsEditorComp::OnAddressEditEnabled(
			const istd::IChangeable::ChangeSet& /*changeSet*/,
			const iprm::IEnableableParam* objectPtr)
{
	if (objectPtr != nullptr){
		HostEdit->setEnabled(objectPtr->IsEnabled());
		PortEdit->setEnabled(objectPtr->IsEnabled());
	}
}


void CDatabaseLoginSettingsEditorComp::OnDatabaseNameEditEnabled(
			const istd::IChangeable::ChangeSet& /*changeSet*/,
			const iprm::IEnableableParam* objectPtr)
{
	if (objectPtr != nullptr){
		DatabaseNameEdit->setEnabled(objectPtr->IsEnabled());
	}
}


void CDatabaseLoginSettingsEditorComp::OnGeneralEditEnabled(
			const istd::IChangeable::ChangeSet& /*changeSet*/,
			const iprm::IEnableableParam* objectPtr)
{
	if (objectPtr != nullptr){
		BaseClass::SetReadOnly(!objectPtr->IsEnabled());
	}
}


} // namespace imtdbgui


