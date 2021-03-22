#include <imtauthgui/CPersonInfoEditorComp.h>


// ACF includes
#include <istd/CChangeGroup.h>


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


} // namespace imtauthgui


