#pragma once


// Qt includes
#include <QtWidgets/QAction>
#include <QtCore/QJsonDocument>


// ACF includes
//#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <iqtgui/IGuiObject.h>
#include <imod/TSingleModelObserverBase.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtauth/IContactInfo.h>
//#include <GeneratedFiles/imtauthgui/ui_CContactInfoEditorComp.h>
#include <imtauth/IContactInfo.h>
#include <imtauthgui/CTreeItemModel.h>
#include <imtqml/CQuickObjectComp.h>


namespace imtauthgui
{


/**
	Editor for the license info.
	\ingroup LicenseManagement
*/
class CContactInfoEditorQmlComp:
		public imtqml::CQuickObjectComp, public iqtgui::IGuiObject, public imod::TSingleModelObserverBase<imtauth::IContactInfo>
//		public imtqml::CQuickObjectComp, public iqtgui::IGuiObject, public imod::TSingleModelObserverBase<imtauth::IContactInfo>
//			public iqtgui::TDesignerGuiObserverCompBase<
//						Ui::CContactInfoEditorComp, imtauth::IContactInfo>
{
//	Q_OBJECT
public:
//	typedef imod::TSingleModelObserverBase<imtauth::IContactInfo> BaseClass;
	typedef imtqml::CQuickObjectComp BaseClass;

	I_BEGIN_COMPONENT(CContactInfoEditorQmlComp);
//		I_REGISTER_INTERFACE(imtauth::IContactInfo);
		I_REGISTER_INTERFACE(imod::IObserver);
		I_REGISTER_INTERFACE(iqtgui::IGuiObject);
		I_ASSIGN(m_guiCompPtr, "GuiCompPtr", "reference to GuiQuickWrap", true, "GuiQuickWrap");
		I_ASSIGN(m_showMailEditAttrPtr, "ShowMailEditor", "Show mail editor", true, true);
		I_ASSIGN(m_showBirthdayEditAttrPtr, "ShowBirthdayEditor", "Show birthday editor", true, true);
		I_ASSIGN(m_showFirstNameEditAttrPtr, "ShowFirstNameEditor", "Show first name editor", true, true);
		I_ASSIGN(m_showLastNameEditAttrPtr, "ShowLastNameEditor", "Show first name editor", true, true);
		I_ASSIGN(m_showNickNameEditAttrPtr, "ShowNickameEditor", "Show last name editor", true, true);
		I_ASSIGN(m_showGenderEditAttrPtr, "ShowGenderEditor", "Show gender editor", true, true);
		I_ASSIGN(m_showAddressesEditAttrPtr, "ShowAddressesEditor", "Show addresses editor", true, true);
	I_END_COMPONENT;

	bool GetJsonContactInfo(QByteArray& jsonContactInfo);

protected:
	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet);
	virtual void OnGuiModelAttached();
	virtual void OnGuiModelDetached();
	virtual void UpdateModel() const;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated();
	virtual void OnGuiDestroyed();

	// reimplemented (iqtgui::IGuiObject)
	virtual bool IsGuiCreated() const override;
	virtual bool CreateGui(QWidget* parentPtr) override;
	virtual bool DestroyGui() override;
	virtual QWidget* GetWidget() const override;
	virtual void OnTryClose(bool* ignoredPtr = NULL) override;
	virtual bool CreateItem(QQmlEngine* enginePtr) override;

//private Q_SLOTS:
	void on_EMailEdit_editingFinished();
	void on_BirthdayEdit_dateChanged(const QDate &date);
	void on_GenderCombo_currentIndexChanged(int index);
	void on_FirstNameEdit_editingFinished();
	void on_LastNameEdit_editingFinished();
	void on_NicknameEdit_editingFinished();
	void on_Addresses_itemSelectionChanged();
//	void on_Addresses_itemChanged(QTreeWidgetItem *item, int column);
	void on_AddAddress_triggered(QAction *action);
	void on_RemoveAddress_triggered(QAction *action);

private:
	QAction m_addAddressAction;
	QAction m_removeAddressAction;
	CTreeItemModel m_treeItemModel;

	I_REF(iqtgui::IGuiObject, m_guiCompPtr);
	I_ATTR(bool, m_showMailEditAttrPtr);
	I_ATTR(bool, m_showBirthdayEditAttrPtr);
	I_ATTR(bool, m_showGenderEditAttrPtr);
	I_ATTR(bool, m_showFirstNameEditAttrPtr);
	I_ATTR(bool, m_showLastNameEditAttrPtr);
	I_ATTR(bool, m_showNickNameEditAttrPtr);
	I_ATTR(bool, m_showAddressesEditAttrPtr);
};


} // namespace imtauthgui


