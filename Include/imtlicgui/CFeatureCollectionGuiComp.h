#pragma once


// ACF includes
#include <ibase/ICommandsProvider.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>
#include <iqtgui/CHierarchicalCommand.h>

// ImtCore includes
#include <imtlic/IFeatureInfoProvider.h>
#include <GeneratedFiles/imtlicgui/ui_CFeatureCollectionGuiComp.h>


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
		I_REGISTER_INTERFACE(ibase::ICommandsProvider);
		I_ASSIGN(m_objectCollectionViewCompPtr, "ObjectCollectionView", "Object collection view", true, "ObjectCollectionView");
	I_END_COMPONENT;

	CFeatureCollectionGuiComp();

protected:
	// reimplemented (ibase::ICommandsProvider)
	virtual const ibase::IHierarchicalCommand* GetCommands() const override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated();
	virtual void OnGuiDestroyed();
	virtual void OnGuiRetranslate() override;

	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void OnGuiModelAttached() override;
	virtual void OnGuiModelDetached() override;
	virtual void UpdateModel() const;

protected Q_SLOTS:
	virtual void OnShowCollectionEditor();
	virtual void OnShowFeatureDependencyEditor();

protected:
	iqtgui::CHierarchicalCommand m_rootCommands;
	iqtgui::CHierarchicalCommand m_showCollectionEditorCommand;
	iqtgui::CHierarchicalCommand m_showDependenciesEditorCommand;

private:
	void UpdateCommands();

private:
	I_REF(iqtgui::IGuiObject, m_objectCollectionViewCompPtr);
};


} // namespace imtlicgui


