#pragma once


// ACF includes
#include <ibase/ICommandsProvider.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <imtlic/IFeatureInfoProvider.h>
#include <GeneratedFiles/imtlicgui/ui_CFeatureInfoProviderGuiComp.h>


namespace imtlicgui
{


/**
	Editor for the product-related licensing.
	\ingroup LicenseManagement
*/
class CFeatureCollectionGuiComp:
			public iqtgui::TDesignerGuiObserverCompBase<
						Ui::CFeatureCollectionGuiComp, imtlic::IFeatureInfoProvider>,
			virtual public ibase::ICommandsProvider
{
	Q_OBJECT

public:
	typedef iqtgui::TDesignerGuiObserverCompBase<
				Ui::CFeatureCollectionGuiComp, imtlic::IFeatureInfoProvider> BaseClass;

	I_BEGIN_COMPONENT(CFeatureCollectionGuiComp);
		I_REGISTER_INTERFACE(ibase::ICommandsProvider)
	I_END_COMPONENT;

	CFeatureCollectionGuiComp();

	// reimplemented (ibase::ICommandsProvider)
	virtual const ibase::IHierarchicalCommand* GetCommands() const override;

protected:
	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void OnGuiModelAttached() override;
	virtual void OnGuiModelDetached() override;
	virtual void UpdateModel() const;
};


} // namespace imtlicgui


