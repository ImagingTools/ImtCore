#pragma once


// Qt includes
#include <QtWidgets/QAction>

// ACF includes
#include <ifile/IFilePersistence.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <iqtgui/IGuiObject.h>

// ImtCore includes
#include <imtauth/IAccountInfo.h>
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
		I_ASSIGN(m_contactCollectionCompPtr, "ContactCollection", "Collection of the contacts", true, "ContactCollection");
		I_ASSIGN_TO(m_contactCollectionModelCompPtr, m_contactCollectionCompPtr, true);
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

private Q_SLOTS:
	void on_ContactCombo_currentIndexChanged(int index);
	void on_AccountTypeCombo_currentIndexChanged(int index);
	void on_AccountNameEdit_editingFinished();
	void on_AccountDescriptionEdit_editingFinished();
	void on_LoadPicture_triggered(QAction *action);
	void on_RemovePicture_triggered(QAction *action);

private:
	void OnContactCollectionUpdate(
				const istd::IChangeable::ChangeSet& changeSet,
				const imtbase::IObjectCollection* objectCollectionPtr);

private:
	I_REF(iqtgui::IGuiObject, m_accountPictureGuiCompPtr);
	I_REF(imod::IObserver, m_accountPictureObserverCompPtr);
	I_REF(imtbase::IObjectCollection, m_contactCollectionCompPtr);
	I_REF(imod::IModel, m_contactCollectionModelCompPtr);
	I_REF(ifile::IFilePersistence, m_bitmapLoaderCompPtr);

	bool m_isContactChangedSignalBlocked;

	QAction m_loadAccountPictureAction;
	QAction m_removeAccountPictureAction;

	imtbase::TModelUpdateBinder<imtbase::IObjectCollection, CAccountInfoEditorComp> m_contactCollectionUpdateBinder;
};


} // namespace imtauthgui


