#pragma once


// Qt includes
#include <QtCore>
#if QT_VERSION >= 0x050000 && QT_VERSION < 0x060000
#include <QtWidgets/QAction>
#else
#include <QtGui/QAction>
#endif

// ACF includes
#include <ifile/IFilePersistence.h>
#include <iimg/CBitmap.h>
#include <imod/CModelProxy.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <iqtgui/IGuiObject.h>

// ImtCore includes
#include <imtauth/IAccountInfo.h>
#include <imtauth/IAddress.h>
#include <imtbase/IObjectCollection.h>
#include <imtbase/TModelUpdateBinder.h>
#include <GeneratedFiles/imtauthgui/ui_CAccountInfoEditorComp.h>


namespace imtauthgui
{


/**
	Editor for the license info.
	\ingroup LicenseManagement
*/
class CAccountInfoEditorComp:
			public iqtgui::TDesignerGuiObserverCompBase<
						Ui::CAccountInfoEditorComp, imtauth::IAccountInfo>
{
	Q_OBJECT
public:
	typedef iqtgui::TDesignerGuiObserverCompBase<
				Ui::CAccountInfoEditorComp, imtauth::IAccountInfo> BaseClass;

	I_BEGIN_COMPONENT(CAccountInfoEditorComp);
		I_ASSIGN(m_accountPictureGuiCompPtr, "AccountPictureView", "Account picture view", true, "ImageView");
		I_ASSIGN_TO(m_accountPictureObserverCompPtr, m_accountPictureGuiCompPtr, true);
		I_ASSIGN(m_personContactEditorCompPtr, "PersonContactEditor", "Person contact editor", true, "ContactInfoEditor");
		I_ASSIGN_TO(m_personContactEditorObserverCompPtr, m_personContactEditorCompPtr, true);
		I_ASSIGN(m_companyContactEditorCompPtr, "CompanyContactEditor", "Company contact editor", true, "ContactInfoEditor");
		I_ASSIGN_TO(m_companyContactEditorObserverCompPtr, m_companyContactEditorCompPtr, true);
		I_ASSIGN(m_bitmapLoaderCompPtr, "BitmapLoader", "Bitmap loader", true, "BitmapLoader");
	I_END_COMPONENT;

	CAccountInfoEditorComp();

protected:
	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void OnGuiModelAttached() override;
	virtual void OnGuiModelDetached() override;
	virtual void UpdateModel() const;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiDestroyed() override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	void SetCompanyAddressVisibility(bool visibility) const;
	void SetupCompanyAddress() const;
	void OnAddressUpdated(const istd::IChangeable::ChangeSet& changeSet, const imtauth::IAddress* addressPtr);
	imtauth::IAddress* GetCompanyAddress();

private Q_SLOTS:
	void on_ContactCombo_currentIndexChanged(int index);
	void on_AccountTypeCombo_currentIndexChanged(int index);
	void on_AccountNameEdit_editingFinished();
	void on_AccountDescriptionEdit_editingFinished();
	void on_CountryEdit_editingFinished();
	void on_CityEdit_editingFinished();
	void on_PostalCodeEdit_editingFinished();
	void on_StreetEdit_editingFinished();

	void on_LoadPicture_triggered(QAction *action);
	void on_RemovePicture_triggered(QAction *action);

private:
	I_REF(iqtgui::IGuiObject, m_accountPictureGuiCompPtr);
	I_REF(imod::IObserver, m_accountPictureObserverCompPtr);
	I_REF(iqtgui::IGuiObject, m_personContactEditorCompPtr);
	I_REF(imod::IObserver, m_personContactEditorObserverCompPtr);
	I_REF(iqtgui::IGuiObject, m_companyContactEditorCompPtr);
	I_REF(imod::IObserver, m_companyContactEditorObserverCompPtr);
	I_REF(ifile::IFilePersistence, m_bitmapLoaderCompPtr);

	QAction m_loadAccountPictureAction;
	QAction m_removeAccountPictureAction;

	imod::CModelProxy m_accountPictureModelProxy;
	imod::TModelWrap<iimg::CBitmap> m_emptyAccountPicture;

	bool m_blockComboChanged;

	mutable imtbase::TModelUpdateBinder<imtauth::IAddress, CAccountInfoEditorComp> m_addressObserver;
};


} // namespace imtauthgui


