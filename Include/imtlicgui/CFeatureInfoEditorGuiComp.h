#pragma once


// ACF includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <imtlic/IFeatureInfo.h>
#include <GeneratedFiles/imtlicgui/ui_CFeatureInfoEditorGuiComp.h>


namespace imtlicgui
{


/**
	Editor for the license info.
	\ingroup LicenseManagement
*/
class CFeatureInfoEditorGuiComp:
			public iqtgui::TDesignerGuiObserverCompBase<
						Ui::CFeatureInfoEditorGuiComp, imtlic::IFeatureInfo>
{
	Q_OBJECT
public:
	typedef iqtgui::TDesignerGuiObserverCompBase<
				Ui::CFeatureInfoEditorGuiComp, imtlic::IFeatureInfo> BaseClass;

	I_BEGIN_COMPONENT(CFeatureInfoEditorGuiComp);
	I_END_COMPONENT;

protected:
	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void UpdateModel() const;

private Q_SLOTS:
	void on_NameEdit_editingFinished();
	void on_IdEdit_editingFinished();
};


} // namespace imtlicgui


