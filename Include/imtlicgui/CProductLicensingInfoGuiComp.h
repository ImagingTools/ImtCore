#pragma once


// ACF includes
#include <ibase/ICommandsProvider.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <imtlic/IProductLicensingInfo.h>
#include <GeneratedFiles/imtlicgui/ui_CProductLicensingInfoGuiComp.h>


namespace imtlicgui
{


/**
	Editor for the product-related licensing.
	\ingroup LicenseManagement
*/
class CProductLicensingInfoGuiComp:
			public iqtgui::TDesignerGuiObserverCompBase<
						Ui::CProductLicensingInfoGuiComp, imtlic::IProductLicensingInfo>,
			virtual public ibase::ICommandsProvider
{
	Q_OBJECT

public:
	typedef iqtgui::TDesignerGuiObserverCompBase<
				Ui::CProductLicensingInfoGuiComp, imtlic::IProductLicensingInfo> BaseClass;

	I_BEGIN_COMPONENT(CProductLicensingInfoGuiComp);
		I_REGISTER_INTERFACE(ibase::ICommandsProvider)
		I_ASSIGN(m_licenseCollectionGuiCompPtr, "LicenseCollectionView", "License collection view", true, "LicenseCollectionView");
		I_ASSIGN_TO(m_licenseCollectionObserverCompPtr, m_licenseCollectionGuiCompPtr, true);
	I_END_COMPONENT;

	CProductLicensingInfoGuiComp();

	// reimplemented (ibase::ICommandsProvider)
	virtual const ibase::IHierarchicalCommand* GetCommands() const override;

protected:
	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void OnGuiModelAttached() override;
	virtual void OnGuiModelDetached() override;
	virtual void UpdateModel() const;

private:
	I_REF(iqtgui::IGuiObject, m_licenseCollectionGuiCompPtr);
	I_REF(imod::IObserver, m_licenseCollectionObserverCompPtr);
};


} // namespace imtlicgui


