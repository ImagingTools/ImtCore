#pragma once


// ACF includes
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <imtlic/IFeatureInfo.h>
#include <imtlicgui/CFeatureDependencyEditorBase.h>
#include <GeneratedFiles/imtlicgui/ui_CFeatureInfoEditorGuiComp.h>


namespace imtlicgui
{


/**
	Editor for the license info.
	\ingroup LicenseManagement
*/
class CFeatureInfoEditorGuiComp:
			public iqtgui::TDesignerGuiObserverCompBase<
						Ui::CFeatureInfoEditorGuiComp, imtlic::IFeatureInfo>,
			protected CFeatureDependencyEditorBase
{
	Q_OBJECT
public:
	typedef iqtgui::TDesignerGuiObserverCompBase<
				Ui::CFeatureInfoEditorGuiComp, imtlic::IFeatureInfo> BaseClass;
	typedef CFeatureDependencyEditorBase BaseClass2;

	I_BEGIN_COMPONENT(CFeatureInfoEditorGuiComp);
	I_END_COMPONENT;

	CFeatureInfoEditorGuiComp();

Q_SIGNALS:
	void EmitFeatureTreeItemChanged();

protected:
	virtual void UpdateFeaturePackageModel() override;
	virtual void FeatureTreeItemChanged() override;

	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void OnGuiModelAttached() override;
	virtual void OnGuiModelDetached() override;
	virtual void UpdateModel() const;

private Q_SLOTS:
	void on_FeatureTree_itemChanged(QTreeWidgetItem *item, int column);
	void on_NameEdit_editingFinished();
	void on_IdEdit_editingFinished();
	virtual void OnFeatureTreeItemChanged();
};


} // namespace imtlicgui


