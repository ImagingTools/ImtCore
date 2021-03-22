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
	void on_GenderCombo_currentIndexChanged(int index);
	void on_BirthdayEdit_dateChanged(const QDate &date);
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
};


} // namespace imtauthgui


