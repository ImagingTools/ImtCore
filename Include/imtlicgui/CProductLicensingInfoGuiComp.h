#pragma once


// ACF includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <imtlic/ILicenseInfo.h>
#include <GeneratedFiles/imtlicgui/ui_CProductLicensingInfoGuiComp.h>


namespace imtlicgui
{


/**
	Editor for the license info.
	\ingroup LicenseManagement
*/
class CProductLicensingInfoGuiComp:
			public iqtgui::TDesignerGuiObserverCompBase<
						Ui::CProductLicensingInfoGuiComp, imtlic::ILicenseInfo>
{
	Q_OBJECT

public:
	typedef iqtgui::TDesignerGuiObserverCompBase<
				Ui::CProductLicensingInfoGuiComp, imtlic::ILicenseInfo> BaseClass;

	I_BEGIN_COMPONENT(CProductLicensingInfoGuiComp);
	I_END_COMPONENT;

	CProductLicensingInfoGuiComp();

protected:
	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void OnGuiModelAttached() override;
	virtual void OnGuiModelDetached() override;
	virtual void UpdateModel() const;

private Q_SLOTS:
};


} // namespace imtlicgui


