#pragma once


// Qt includes
#include <QtWidgets/QAction>


// ACF includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <imtauth/IContactInfo.h>
#include <GeneratedFiles/imtauthgui/ui_CContactInfoEditorComp.h>


namespace imtauthgui
{


/**
	Editor for the license info.
	\ingroup LicenseManagement
*/
class CContactInfoEditorComp:
			public iqtgui::TDesignerGuiObserverCompBase<
						Ui::CContactInfoEditorComp, imtauth::IContactInfo>
{
	Q_OBJECT
public:
	typedef iqtgui::TDesignerGuiObserverCompBase<
				Ui::CContactInfoEditorComp, imtauth::IContactInfo> BaseClass;

	I_BEGIN_COMPONENT(CContactInfoEditorComp);
		I_ASSIGN(m_showMailEditAttrPtr, "ShowMailEditor", "Show mail editor", true, true);
		I_ASSIGN(m_showBirthdayEditAttrPtr, "ShowBirthdayEditor", "Show birthday editor", true, true);
		I_ASSIGN(m_showFirstNameEditAttrPtr, "ShowFirstNameEditor", "Show first name editor", true, true);
		I_ASSIGN(m_showLastNameEditAttrPtr, "ShowLastNameEditor", "Show first name editor", true, true);
		I_ASSIGN(m_showNickNameEditAttrPtr, "ShowNickameEditor", "Show last name editor", true, true);
		I_ASSIGN(m_showGenderEditAttrPtr, "ShowGenderEditor", "Show gender editor", true, true);
		I_ASSIGN(m_showAddressesEditAttrPtr, "ShowAddressesEditor", "Show addresses editor", true, true);
	I_END_COMPONENT;

protected:
	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void OnGuiModelAttached() override;
	virtual void OnGuiModelDetached() override;
	virtual void UpdateModel() const;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiDestroyed() override;

private Q_SLOTS:
	void on_EMailEdit_editingFinished();
	void on_BirthdayEdit_dateChanged(const QDate &date);
	void on_GenderCombo_currentIndexChanged(int index);
	void on_FirstNameEdit_editingFinished();
	void on_LastNameEdit_editingFinished();
	void on_NicknameEdit_editingFinished();
	void on_Addresses_itemSelectionChanged();
	void on_Addresses_itemChanged(QTreeWidgetItem *item, int column);
	void on_AddAddress_triggered(QAction *action);
	void on_RemoveAddress_triggered(QAction *action);

private:
	QAction m_addAddressAction;
	QAction m_removeAddressAction;

	I_ATTR(bool, m_showMailEditAttrPtr);
	I_ATTR(bool, m_showBirthdayEditAttrPtr);
	I_ATTR(bool, m_showGenderEditAttrPtr);
	I_ATTR(bool, m_showFirstNameEditAttrPtr);
	I_ATTR(bool, m_showLastNameEditAttrPtr);
	I_ATTR(bool, m_showNickNameEditAttrPtr);
	I_ATTR(bool, m_showAddressesEditAttrPtr);
};


} // namespace imtauthgui


