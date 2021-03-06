#pragma once


// ACF includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <imtbase/TModelUpdateBinder.h>
#include <imtbase/IObjectCollection.h>
#include <imtlic/IProductInstanceInfo.h>
#include <GeneratedFiles/imtlicgui/ui_CProductInstanceInfoEditorComp.h>


namespace imtlicgui
{


/**
	Editor for the license info.
	\ingroup LicenseManagement
*/
class CProductInstanceInfoEditorComp:
			public iqtgui::TDesignerGuiObserverCompBase<
						Ui::CProductInstanceInfoEditorComp, imtlic::IProductInstanceInfo>
{
	Q_OBJECT

public:
	typedef iqtgui::TDesignerGuiObserverCompBase<
				Ui::CProductInstanceInfoEditorComp, imtlic::IProductInstanceInfo> BaseClass;

	I_BEGIN_COMPONENT(CProductInstanceInfoEditorComp);
	I_END_COMPONENT;

	CProductInstanceInfoEditorComp();

protected:
	void OnProductsUpdated(const istd::IChangeable::ChangeSet& changeSet, const imtbase::IObjectCollection* productCollectionPtr);
	void OnLicensesUpdated(const istd::IChangeable::ChangeSet& changeSet, const imtbase::IObjectCollection* productCollectionPtr);
	void UpdateProductsCombo();
	void UpdateLicensesCombo();

	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void OnGuiModelAttached() override;
	virtual void OnGuiModelDetached() override;
	virtual void UpdateModel() const;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiDestroyed() override;

private Q_SLOTS:
	void on_ProductInstanceIdEdit_editingFinished();
	void on_ProductCombo_currentIndexChanged(int index);
	void on_LicenseCombo_currentIndexChanged(int index);
	void on_ValidUntilDate_dateTimeChanged(const QDateTime& dateTime);
	void on_ExpireGroup_toggled(bool toggled);

private:
	imtbase::TModelUpdateBinder<imtbase::IObjectCollection, CProductInstanceInfoEditorComp> m_productCollectionObserver;
	imtbase::TModelUpdateBinder<imtbase::IObjectCollection, CProductInstanceInfoEditorComp> m_licenseCollectionObserver;
};


} // namespace imtlicgui


