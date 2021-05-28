#pragma once


// ACF includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <imtlic/IProductInstanceInfo.h>
#include <GeneratedFiles/imtlicgui/ui_CProductInstanceInfoViewComp.h>


namespace imtlicgui
{


/**
	Editor for the license info.
	\ingroup LicenseManagement
*/
class CProductInstanceInfoViewComp:
			public iqtgui::TDesignerGuiObserverCompBase<
						Ui::CProductInstanceInfoViewComp, imtlic::IProductInstanceInfo>
{
	Q_OBJECT

public:
	typedef iqtgui::TDesignerGuiObserverCompBase<
				Ui::CProductInstanceInfoViewComp, imtlic::IProductInstanceInfo> BaseClass;

	I_BEGIN_COMPONENT(CProductInstanceInfoViewComp);
	I_END_COMPONENT;

	CProductInstanceInfoViewComp();

	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiDestroyed() override;

private:
	void UpdateProductName();
	void UpdateFeatureTree();
};


} // namespace imtlicgui


